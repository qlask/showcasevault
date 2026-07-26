# Showcase Vault

A single-page, fan-made archive site for downloading Geometry Dash level files (`.gdr`, `.gdr2`, `.gmd`) from showcase creators. No accounts, no upload form, no backend — everything lives in one static HTML file.

## What it does

- **Browse** a grid of levels with auto-generated-style previews, difficulty badges, object counts, and file size.
- **Search & filter** levels by name/creator and by file type (`.gdr`, `.gdr2`, `.gmd`).
- **Level detail pages** at a dedicated URL per level (`#/<slug>`), showing full stats, description, technical name, upload date, and a download button.
- **Copy level ID** — every level shows its numeric in-game ID as plain text next to a clipboard button, so visitors can copy it with one click.
- **Download tracking** — each download increments a per-level counter and a sitewide total, persisted in the browser via `localStorage`.
- **Changelog page** listing every addition/fix/change to the site.
- **Request a showcase** button that opens a pre-filled `mailto:` link — this is the *only* way anyone outside the source code can propose a level.

## How it's built

Everything — markup, styles, and logic — is in a single file: `showcase-vault.html`. There is no build step, no framework, and no server. Just open the file in a browser.

- Vanilla HTML/CSS/JS, no dependencies except Google Fonts (Space Grotesk, Inter, JetBrains Mono).
- Client-side hash router (`#browse`, `#changelog`, `#/<slug>`) swaps between three page sections without a page reload.
- Download counts are stored in `localStorage` under the key `showcase_vault_downloads`, so counts are local to each visitor's browser (not shared globally).

## Adding or editing levels

There is no admin panel or upload form by design — the vault is hand-curated. To add, edit, or remove a level, edit the `LEVELS` array directly in the `<script>` section of `showcase-vault.html`. The grid, search, filters, and detail pages all update automatically from this array.

Each entry looks like this:

```js
{
  slug:"-",              // unique, lowercase, hyphenated — used in the detail page URL (#/blackpill)
  name:"-",
  creator:"-",
  type:"-",                   // "gdr" | "gdr2" | "gmd"
  size:"- B/KB",
  objects:"-",
  diff:"-",
  gif:"https://i.imgur.com/-.png",   // preview image URL
  file:"https://drive.google.com/...",     // download link
  description:"-.",   // optional, shown on the detail page
  id:"-",                          // level's in-game ID — shown as copyable text, not a link
  uploadDate:"????-??-??"                  // optional, "YYYY-MM-DD" — shown under Technical name on the detail page
}
```

Notes:
- `slug` must be unique — it's what makes each level's detail page linkable.
- `id` is display-only (shown next to a copy-to-clipboard button); it isn't used to build any URL.
- `uploadDate` is optional — omit it (or leave it `""`) if the upload date isn't known, and the line simply won't render.

## Adding a changelog entry

Edit the `CHANGELOG` array the same way, adding new entries to the **top** of the array (they render newest-first):

```js
{
  date:"2026-07-26",     // "YYYY-MM-DD"
  tag:"added",           // "added" | "fixed" | "changed" — controls the pill color
  title:"Short title",
  body:"One or two sentences describing the change."
}
```

## Contact / showcase requests

The footer, hero, and nav "Request a Level Showcase" buttons all open the same pre-filled email draft, so the maintainer can review submissions manually before adding them to `LEVELS`.

## Disclaimer

Fan-made archive. Not affiliated with RobTop Games.
