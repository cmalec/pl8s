---
name: pebble-release
description: |
  Publish a .pbw release to the Rebble appstore without the browser: the exact dev-portal API call (POST of release_notes + the .pbw to appstore-api.rebble.io), credential handling in a gitignored .env, the pre-flight checks that stop a stale build being published under a new version, and the local capture-server recipe for testing the upload without publishing anything. Use when the user asks to release, publish or upload a build to the Pebble/Rebble appstore, to automate dev-portal uploads, or to script a release from the command line.
---

# Publishing a release to the Rebble appstore

The dev portal's release dialog is a single POST: multipart form data
(`release_notes` + the `.pbw`) to
`https://appstore-api.rebble.io/api/dp/app/<app id>/release`, authorised with the
portal's bearer token. `scripts/release.sh` sends exactly that, so a release is a
command instead of a browser session. Contract, verified facts and unknowns:
[`reference.md`](./reference.md).

## Setup, once per machine

1. Token: dev-portal.rebble.io → DevTools → Network → any `/api/dp/…` request →
   `Authorization: Bearer …`.
2. App id: last path segment of the appstore URL,
   `https://apps.rebble.io/en_US/application/<app id>`.
3. Put both in the project's `.env` (never committed; `chmod 600`):

   ```sh
   printf 'REBBLE_API_TOKEN=%s\nREBBLE_APP_ID=%s\n' "$token" "$id" > .env
   grep -qxF .env .gitignore || echo .env >> .gitignore
   ```

A `.env.example` with the two empty keys belongs in the repo; the `.env` does
not. Exported environment variables win over `.env`, so CI can inject the token
without a file.

## Workflow

```sh
jq '.version' package.json                 # bump it first
pebble build
unzip -p build/<name>.pbw appinfo.json | jq '{versionLabel, uuid, targetPlatforms}'
cp build/<name>.pbw releases/<name>-v<version>.pbw     # keep releases in git
.agent/skills/pebble-release/scripts/release.sh --dry-run --notes "..."
.agent/skills/pebble-release/scripts/release.sh --notes "..."
```

With no `.pbw` argument the script uploads
`releases/<name>-v<package.json version>.pbw`; the project root is the nearest
directory above the script with a `pebble` block in `package.json`, else the
current directory.

Release notes are required: `--notes "..."` or `--notes-file path` (the file
keeps multi-line notes usable). One line, verb-first, matching the repo's commit
subject is the style that reads well in the appstore.

## What the script refuses

| Check | Why |
|---|---|
| `.pbw` missing, empty, or not a zip with `appinfo.json` | a truncated build would otherwise be uploaded |
| `appinfo.json` `versionLabel` ≠ `package.json` `version` | the stale-build bug: build once, bump, upload the old binary |
| `appinfo.json` `uuid` ≠ `package.json` `pebble.uuid` | wrong project's build |
| empty/whitespace release notes | the portal shows them; a published empty note needs a new release to fix |
| non-2xx response | prints the API body, exits 1 |
| `--dry-run` | prints URL, masked token, notes and byte count, sends nothing |

Naming a file outside `releases/<name>-v<version>.pbw` is only a warning, so an
explicit path still works.

## Verify a release

- The script prints the HTTP status and the JSON the API returned.
- Open <https://dev-portal.rebble.io/> and confirm the release is listed at the
  expected version **and published** - uploading and publishing are separate
  states in the portal as far as this skill has observed.
- Install the published version from the appstore on a real watch: that is the
  only check of the phone-app paths (settings gear, companion app).

## Bundled files

| File | Use |
|---|---|
| `scripts/release.sh` | uploads a `.pbw` with notes; `--dry-run`; validated inputs |
| `reference.md` | request contract, credential rules, response and failure modes, capture-server test recipe |

Listing assets (icons, screenshots, descriptions) are a different job: see the
`pebble-appstore` skill.
