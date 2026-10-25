#!/usr/bin/env bash
# Publish a release build to the Rebble appstore dev portal.
#
# What the portal's "upload .pbw" button does, minus the browser: one POST of
# multipart form data (release_notes + the .pbw) to
#   <api>/api/dp/app/<app id>/release
# with the dev-portal bearer token.
#
# Usage, from inside a Pebble project:
#   .agent/skills/pebble-release/scripts/release.sh --notes "what changed"
#   ...scripts/release.sh --notes-file notes.txt releases/pl8s-v1.1.0.pbw
#   ...scripts/release.sh --dry-run --notes "what changed"   # send nothing
#
# With no .pbw argument it uses releases/<name>-v<package.json version>.pbw.
# The project root is the nearest directory above this script holding a
# package.json with a pebble block, else the current directory.
#
# Credentials live in the gitignored project .env:
#   REBBLE_API_TOKEN=<dev-portal bearer token>
#   REBBLE_APP_ID=<appstore app id, the last path segment of the portal URL>
# Already-exported environment variables win over .env. REBBLE_API_BASE
# overrides the API host (only needed for testing).
set -euo pipefail

prog=${0##*/}

die() {
  printf '%s: %s\n' "$prog" "$*" >&2
  exit 1
}

usage() {
  awk 'NR > 1 && /^#/ { sub(/^# ?/, ""); print; next } NR > 1 { exit }' \
    "${BASH_SOURCE[0]}" >&2
  exit "${1:-0}"
}

project_root() {
  local dir
  dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
  while [[ $dir != / ]]; do
    if [[ -f $dir/package.json ]] &&
      jq -e '.pebble' "$dir/package.json" >/dev/null 2>&1; then
      printf '%s\n' "$dir"
      return 0
    fi
    dir=$(dirname -- "$dir")
  done
  pwd
}

load_env() {
  local file=$1 line key val
  [[ -f $file ]] || return 0
  while IFS= read -r line || [[ -n $line ]]; do
    line=${line%$'\r'}
    case $line in '' | '#'*) continue ;; esac
    [[ $line == *=* ]] || die "$file: not KEY=value: $line"
    key=${line%%=*}
    val=${line#*=}
    [[ $key =~ ^[A-Za-z_][A-Za-z0-9_]*$ ]] || die "$file: bad variable name: $key"
    case $val in
      \"*\") val=${val:1:${#val}-2} ;;
      \'*\') val=${val:1:${#val}-2} ;;
    esac
    # An exported value is a deliberate override (CI, one-off run).
    [[ -n ${!key-} ]] || export "$key=$val"
  done <"$file"
}

notes=
notes_file=
dry_run=0
pbw=

while (($#)); do
  case $1 in
    -n | --notes) (($# >= 2)) || die "$1 needs a value"; notes=$2; shift 2 ;;
    -f | --notes-file) (($# >= 2)) || die "$1 needs a value"; notes_file=$2; shift 2 ;;
    --dry-run) dry_run=1; shift ;;
    -h | --help) usage 0 ;;
    -*) die "unknown option: $1 (try --help)" ;;
    *) [[ -z $pbw ]] || die "only one .pbw, got '$pbw' and '$1'"; pbw=$1; shift ;;
  esac
done

command -v curl >/dev/null || die "curl is required"
command -v unzip >/dev/null || die "unzip is required"
command -v jq >/dev/null || die "jq is required"

root=$(project_root)
load_env "$root/.env"

[[ -n ${REBBLE_APP_ID-} ]] || die "REBBLE_APP_ID is unset (add it to $root/.env)"
[[ -n ${REBBLE_API_TOKEN-} ]] || die "REBBLE_API_TOKEN is unset (add it to $root/.env)"
api_base=${REBBLE_API_BASE:-https://appstore-api.rebble.io}

pkg_version=$(jq -er .version "$root/package.json") || die "package.json has no version"
pkg_uuid=$(jq -er .pebble.uuid "$root/package.json") || die "package.json has no pebble.uuid"
pkg_name=$(jq -er .name "$root/package.json") || die "package.json has no name"

pbw=${pbw:-"$root/releases/$pkg_name-v$pkg_version.pbw"}
[[ -f $pbw ]] || die "no such .pbw: $pbw (build it with 'pebble build' and copy it into releases/)"
[[ -s $pbw ]] || die "$pbw is empty"

appinfo=$(unzip -p "$pbw" appinfo.json 2>/dev/null) || die "$pbw is not a .pbw (no appinfo.json)"
pbw_version=$(jq -er .versionLabel <<<"$appinfo") || die "$pbw has no versionLabel"
pbw_uuid=$(jq -er .uuid <<<"$appinfo") || die "$pbw has no uuid"

[[ $pbw_version == "$pkg_version" ]] ||
  die "$pbw is version $pbw_version but package.json says $pkg_version (pebble build after bumping)"
[[ ${pbw_uuid,,} == "${pkg_uuid,,}" ]] ||
  die "$pbw uuid $pbw_uuid does not match package.json uuid $pkg_uuid"
[[ $(basename -- "$pbw") == "$pkg_name-v$pbw_version.pbw" ]] ||
  printf '%s: warning: %s is not named %s-v%s.pbw\n' "$prog" "$(basename -- "$pbw")" "$pkg_name" "$pbw_version" >&2

if [[ -n $notes_file ]]; then
  [[ -z $notes ]] || die "use --notes or --notes-file, not both"
  [[ -f $notes_file ]] || die "no such notes file: $notes_file"
  notes=$(<"$notes_file")
fi
[[ -n ${notes//[[:space:]]/} ]] || die "release notes are required: --notes \"...\" or --notes-file file"

url="$api_base/api/dp/app/$REBBLE_APP_ID/release"
notes=${notes%$'\n'}

# Origin/Referer are not needed for auth; they match what the portal sends, so
# a request from this script looks like the browser's and cannot trip a check
# that only exists on that path.
curl_args=(
  -sS
  -X POST
  -H "Authorization: Bearer $REBBLE_API_TOKEN"
  -H 'Accept: application/json'
  -H 'Origin: https://dev-portal.rebble.io'
  -H 'Referer: https://dev-portal.rebble.io/'
  --form-string "release_notes=$notes"
  -F "pbw=@$pbw;type=application/octet-stream"
)

if ((dry_run)); then
  printf 'POST %s\n' "$url"
  printf '  Authorization: Bearer %s…\n' "${REBBLE_API_TOKEN:0:6}"
  printf '  release_notes: %s\n' "${notes//$'\n'/ }"
  printf '  pbw: %s (%s bytes, version %s)\n' "$pbw" "$(wc -c <"$pbw" | tr -d ' ')" "$pbw_version"
  exit 0
fi

body=$(mktemp)
trap 'rm -f "$body"' EXIT

printf '%s: uploading %s (v%s, %s bytes) to app %s\n' \
  "$prog" "$(basename -- "$pbw")" "$pbw_version" "$(wc -c <"$pbw" | tr -d ' ')" "$REBBLE_APP_ID" >&2
status=$(curl "${curl_args[@]}" -o "$body" -w '%{http_code}' "$url") || die "request failed (is the API reachable?)"

if [[ $status != 2* ]]; then
  printf '%s: HTTP %s from %s\n' "$prog" "$status" "$url" >&2
  if jq -e . "$body" >/dev/null 2>&1; then jq . "$body" >&2; else cat "$body" >&2; fi
  exit 1
fi

printf '%s: HTTP %s - release v%s uploaded\n' "$prog" "$status" "$pbw_version" >&2
if jq -e . "$body" >/dev/null 2>&1 && [[ -s $body ]]; then jq . "$body" >&2; fi
printf 'Confirm the release and its "published" state at https://dev-portal.rebble.io/\n' >&2
