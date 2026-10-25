# Release API reference

## The request

Captured from dev-portal.rebble.io's release dialog (browser DevTools, "Copy as
cURL"), reproduced by `scripts/release.sh`:

```
POST https://appstore-api.rebble.io/api/dp/app/<app id>/release
Authorization: Bearer <token>
Content-Type: multipart/form-data; boundary=…
Accept: */*

release_notes   text field, the changelog the appstore shows
pbw             file field, filename="<name>-v<version>.pbw",
                Content-Type: application/octet-stream
```

The portal also sends `Origin`/`Referer: https://dev-portal.rebble.io/`,
`Sec-Fetch-*`, `DNT`, `TE` and a Firefox user agent. The script keeps
`Origin`/`Referer` (cheap, and removes any doubt about a same-site check) and
drops the browser-fingerprint headers. Auth is the bearer token alone.

`<app id>` is the appstore's own id (a 24-hex MongoDB-style id), **not** the
watchapp UUID from `package.json`. It is the last segment of
`https://apps.rebble.io/en_US/application/<app id>` and is stable across
releases.

## Verified facts

- `GET` on `/api/dp/app/<id>` and `/api/dp/app/<id>/release` returns
  `405 Method Not Allowed`; `/api/dp/app` returns 404. The route exists and is
  POST-only, so the script's URL is right.
- The multipart body the script builds parses back with the field names above,
  the pbw part byte-identical (sha256 matched) to the file on disk, and the
  bearer header intact - checked against a local capture server (below).

## Unknowns, deliberately not smuggled in as behaviour

- What the 2xx body contains (the script pretty-prints it if it is JSON).
- Whether the POST publishes the release or leaves it unpublished: the portal
  keeps "release uploaded" and "release published" as separate states, so check
  the portal after a run.
- Release-notes length limit and whether the API rejects duplicate versions;
  the script surfaces the error body rather than guessing.
- Whether a beta SDK build is rejected server-side (the portal's own guide says
  non-beta only).

## Credentials

- Token and app id live in the project `.env`, which is gitignored, `chmod 600`.
  `.env.example` documents the two keys and nothing else.
- Exported env vars beat `.env` (CI, one-off runs); `REBBLE_API_BASE` overrides
  the host and exists for tests.
- The token is a dev-portal session credential: it is worth rotating in the
  portal if the file is ever shared, and it must never appear in a commit, a
  release note, or a `--dry-run` line (the dry run masks it after 6 chars).

## Pre-flight, before the script runs

- Version in `package.json` above every published release - the appstore needs a
  higher version, and a rebuilt same-version pbw is rejected or ignored.
- `pebble build` for every platform in `targetPlatforms`, not just one, or the
  pbw ships stale binaries for the rest.
- Build from a released (non-beta) SDK.
- `releases/<name>-v<version>.pbw` committed so the binary that the appstore is
  serving stays in git forever.

## Testing the upload without publishing

`REBBLE_API_BASE` points the script at a local server, which is how the
multipart contract was checked:

```sh
python3 - http://127.0.0.1:8731 <<'PY' &
import sys, json, hashlib
from email.parser import BytesParser
from email.policy import HTTP
from http.server import BaseHTTPRequestHandler, HTTPServer

class H(BaseHTTPRequestHandler):
    def do_POST(self):
        raw = self.rfile.read(int(self.headers["Content-Length"]))
        msg = BytesParser(policy=HTTP).parsebytes(
            b"Content-Type: " + self.headers["Content-Type"].encode() + b"\r\n\r\n" + raw)
        out = {"path": self.path, "auth": self.headers.get("Authorization"), "parts": {}}
        for part in msg.iter_parts():
            payload = part.get_payload(decode=True)
            out["parts"][part.get_param("name", header="content-disposition")] = {
                "filename": part.get_filename(), "len": len(payload),
                "sha": hashlib.sha256(payload).hexdigest(),
                "text": payload.decode("utf-8", "replace")}
        print(json.dumps(out, indent=2), flush=True)
        self.send_response(200); self.end_headers()
        self.wfile.write(b'{"_id":"fake"}')
    def log_message(self, *a): pass

HTTPServer(("127.0.0.1", 8731), H).serve_forever()
PY
REBBLE_API_BASE=http://127.0.0.1:8731 scripts/release.sh --notes smoke
```

Compare the captured `pbw` sha256 against `shasum -a 256 <file>` and the path
against `/api/dp/app/<app id>/release`. Exercise the failure paths too: a
non-2xx from the server must print the body and exit 1, and a pbw whose
`versionLabel` is behind `package.json` must never reach the network.

## Failure modes seen while writing the script

| Symptom | Cause |
|---|---|
| `release.sh: … is version X but package.json says Y` | uploaded the wrong file: build after the bump, copy again |
| `HTTP 401` in the printed body | token expired/rotated: grab a fresh one from the portal |
| `curl: (7) Failed to connect` + `request failed` | wrong `REBBLE_API_BASE`, offline, or a proxy |
| Notes empty | `--notes` not passed and no `releases/` default exists: the portal does not invent a changelog |
