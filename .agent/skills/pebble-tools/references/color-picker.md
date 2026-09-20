# Color Picker Tool

Source: https://developer.repebble.com/guides/tools-and-resources/color-picker/

> **Note (transcription provenance).** This page is JavaScript-driven: in reader mode
> only the intro sentence, an empty information table, and the "Example Code Segment"
> heading render, so the interactive parts below are transcribed from the page's own
> JavaScript assets and the raw page HTML:
>
> - https://developer.repebble.com/assets/js/tools/color-dict.js (the per-color data)
> - https://developer.repebble.com/assets/js/tools/color-picker.js (interaction)
> - https://developer.repebble.com/assets/js/tools/color-mapping-sunlight.js (sunlight map)
> - https://developer.repebble.com/assets/js/templates.js (the sample code template)
>
> The inline SVG color map is preserved exactly as embedded in the page (as a data URI,
> so this copy is viewable offline). Everything not from the page itself is explicitly
> labeled as coming from the local SDK (see "SDK supplement" at the end).
>
> In the color list, parentheses inside link targets are percent-encoded (%28/%29) so
> the markdown links remain valid; the encoded URLs resolve to the same pages.

Click a hexagon in the color map to see its SDK constant and how to use
it in your Pebble app.

## The Color Map

The color map contains one hexagon for each of the 64 colors available on Pebble, each
filled with its uncorrected HTML color. Clicking a hexagon selects it and fills in the
information table below. The map is shown below as embedded in the page (as an inline
SVG data URI).

![Hexagonal color map of all 64 Pebble colors](data:image/svg+xml;base64,PHN2ZyB2ZXJzaW9uPSIxLjEiIHhtbG5zPSJodHRwOi8vd3d3LnczLm9yZy8yMDAwL3N2ZyIgeG1sbnM6eGxpbms9Imh0dHA6Ly93d3cudzMub3JnLzE5OTkveGxpbmsiIHg9IjBweCIgeT0iMHB4IgoJIHZpZXdCb3g9IjQzNSAyNDAgMzU1IDM0NSIgZW5hYmxlLWJhY2tncm91bmQ9Im5ldyA0NDguMSAyNTEuNSAzMjkuMiAzMjMuOCIgeG1sOnNwYWNlPSJwcmVzZXJ2ZSIgaWQ9ImNvbG9yLXBpY2tlciIgY2xhc3M9ImNvbG9yLXBpY2tlciI+Cgk8cG9seWdvbiBzdHJva2U9IiNGRkZGRkYiIHN0cm9rZS13aWR0aD0iMS41IiBmaWxsPSIjMDAwMDAwIiBwb2ludHM9IjcyOC40LDI3OS40IDc0NC45LDI4OSA3NDQuOSwzMDguMSA3MjguNCwzMTcuNyA3MTEuOCwzMDguMSA3MTEuOCwyODkgCSIvPgoJPHBvbHlnb24gc3Ryb2tlPSIjRkZGRkZGIiBzdHJva2Utd2lkdGg9IjEuNSIgZmlsbD0iIzAwMDA1NSIgcG9pbnRzPSI2MjkuMiw0NTEuNiA2NDUuNyw0NjEuMiA2NDUuNyw0ODAuMyA2MjkuMiw0ODkuOSA2MTIuNyw0ODAuMyA2MTIuNyw0NjEuMiAJIi8+Cgk8cG9seWdvbiBzdHJva2U9IiNGRkZGRkYiIHN0cm9rZS13aWR0aD0iMS41IiBmaWxsPSIjMDAwMEFBIiBwb2ludHM9IjU5Ni4yLDQ1MS42IDYxMi43LDQ2MS4yIDYxMi43LDQ4MC4zIDU5Ni4yLDQ4OS45IDU3OS43LDQ4MC4zIDU3OS43LDQ2MS4yIAkiLz4KCTxwb2x5Z29uIHN0cm9rZT0iI0ZGRkZGRiIgc3Ryb2tlLXdpZHRoPSIxLjUiIGZpbGw9IiMwMDAwRkYiIHBvaW50cz0iNTc5LjcsNDgwLjMgNTk2LjIsNDg5LjkgNTk2LjIsNTA5IDU3OS43LDUxOC42IDU2My4xLDUwOSA1NjMuMSw0ODkuOSAJIi8+Cgk8cG9seWdvbiBzdHJva2U9IiNGRkZGRkYiIHN0cm9rZS13aWR0aD0iMS41IiBmaWxsPSIjMDA1NTAwIiBwb2ludHM9IjU3OS43LDM2NS41IDU5Ni4yLDM3NS4xIDU5Ni4yLDM5NC4yIDU3OS43LDQwMy44IDU2My4xLDM5NC4yIDU2My4xLDM3NS4xIAkiLz4KCTxwb2x5Z29uIHN0cm9rZT0iI0ZGRkZGRiIgc3Ryb2tlLXdpZHRoPSIxLjUiIGZpbGw9IiMwMDU1NTUiIHBvaW50cz0iNTk2LjIsMzk0LjIgNjEyLjcsNDAzLjggNjEyLjcsNDIyLjkgNTk2LjIsNDMyLjUgNTc5LjcsNDIyLjkgNTc5LjcsNDAzLjggCSIvPgoJPHBvbHlnb24gc3Ryb2tlPSIjRkZGRkZGIiBzdHJva2Utd2lkdGg9IjEuNSIgZmlsbD0iIzAwNTVBQSIgcG9pbnRzPSI1NzkuNyw0MjIuOSA1OTYuMiw0MzIuNSA1OTYuMiw0NTEuNiA1NzkuNyw0NjEuMiA1NjMuMSw0NTEuNiA1NjMuMSw0MzIuNSAJIi8+Cgk8cG9seWdvbiBzdHJva2U9IiNGRkZGRkYiIHN0cm9rZS13aWR0aD0iMS41IiBmaWxsPSIjMDA1NUZGIiBwb2ludHM9IjU2My4xLDUwOSA1NzkuNyw1MTguNiA1NzkuNyw1MzcuNyA1NjMuMSw1NDcuMyA1NDYuNiw1MzcuNyA1NDYuNiw1MTguNiAJIi8+Cgk8cG9seWdvbiBzdHJva2U9IiNGRkZGRkYiIHN0cm9rZS13aWR0aD0iMS41IiBmaWxsPSIjMDBBQTAwIiBwb2ludHM9IjUzMC4xLDMzNi45IDU0Ni42LDM0Ni40IDU0Ni42LDM2NS41IDUzMC4xLDM3NS4xIDUxMy42LDM2NS42IDUxMy42LDM0Ni40IAkiLz4KCTxwb2x5Z29uIHN0cm9rZT0iI0ZGRkZGRiIgc3Ryb2tlLXdpZHRoPSIxLjUiIGZpbGw9IiMwMEFBNTUiIHBvaW50cz0iNTEzLjYsMzY1LjYgNTMwLjEsMzc1LjEgNTMwLjEsMzk0LjIgNTEzLjYsNDAzLjggNDk3LDM5NC4zIDQ5NywzNzUuMSAJIi8+Cgk8cG9seWdvbiBzdHJva2U9IiNGRkZGRkYiIHN0cm9rZS13aWR0aD0iMS41IiBmaWxsPSIjMDBBQUFBIiBwb2ludHM9IjUzMC4xLDM5NC4yIDU0Ni42LDQwMy44IDU0Ni42LDQyMi45IDUzMC4xLDQzMi41IDUxMy42LDQyMi45IDUxMy42LDQwMy44IAkiLz4KCTxwb2x5Z29uIHN0cm9rZT0iI0ZGRkZGRiIgc3Ryb2tlLXdpZHRoPSIxLjUiIGZpbGw9IiMwMEFBRkYiIHBvaW50cz0iNTYzLjEsNDUxLjYgNTc5LjcsNDYxLjIgNTc5LjcsNDgwLjMgNTYzLjEsNDg5LjkgNTQ2LjYsNDgwLjMgNTQ2LjYsNDYxLjIgCSIvPgoJPHBvbHlnb24gc3Ryb2tlPSIjRkZGRkZGIiBzdHJva2Utd2lkdGg9IjEuNSIgZmlsbD0iIzAwRkYwMCIgcG9pbnRzPSI1MTMuNiwzMDguMiA1MzAuMSwzMTcuNyA1MzAuMSwzMzYuOSA1MTMuNiwzNDYuNCA0OTcsMzM2LjkgNDk3LDMxNy43IAkiLz4KCTxwb2x5Z29uIHN0cm9rZT0iI0ZGRkZGRiIgc3Ryb2tlLXdpZHRoPSIxLjUiIGZpbGw9IiMwMEZGNTUiIHBvaW50cz0iNDk3LDMzNi45IDUxMy42LDM0Ni40IDUxMy42LDM2NS42IDQ5NywzNzUuMSA0ODAuNSwzNjUuNiA0ODAuNSwzNDYuNCAJIi8+Cgk8cG9seWdvbiBzdHJva2U9IiNGRkZGRkYiIHN0cm9rZS13aWR0aD0iMS41IiBmaWxsPSIjMDBGRkFBIiBwb2ludHM9IjQ5NywzOTQuMyA1MTMuNiw0MDMuOCA1MTMuNiw0MjIuOSA0OTcsNDMyLjUgNDgwLjUsNDIzIDQ4MC41LDQwMy44IAkiLz4KCTxwb2x5Z29uIHN0cm9rZT0iI0ZGRkZGRiIgc3Ryb2tlLXdpZHRoPSIxLjUiIGZpbGw9IiMwMEZGRkYiIHBvaW50cz0iNTEzLjYsNDIyLjkgNTMwLjEsNDMyLjUgNTMwLjEsNDUxLjYgNTEzLjYsNDYxLjIgNDk3LDQ1MS42IDQ5Nyw0MzIuNSAJIi8+Cgk8cG9seWdvbiBzdHJva2U9IiNGRkZGRkYiIHN0cm9rZS13aWR0aD0iMS41IiBmaWxsPSIjNTUwMDAwIiBwb2ludHM9IjY3OC44LDQyMi45IDY5NS4zLDQzMi41IDY5NS4zLDQ1MS42IDY3OC44LDQ2MS4yIDY2Mi4zLDQ1MS42IDY2Mi4zLDQzMi41IAkiLz4KCTxwb2x5Z29uIHN0cm9rZT0iI0ZGRkZGRiIgc3Ryb2tlLXdpZHRoPSIxLjUiIGZpbGw9IiM1NTAwNTUiIHBvaW50cz0iNjYyLjMsNDUxLjYgNjc4LjgsNDYxLjIgNjc4LjgsNDgwLjMgNjYyLjMsNDg5LjkgNjQ1LjcsNDgwLjMgNjQ1LjcsNDYxLjIgCSIvPgoJPHBvbHlnb24gc3Ryb2tlPSIjRkZGRkZGIiBzdHJva2Utd2lkdGg9IjEuNSIgZmlsbD0iIzU1MDBBQSIgcG9pbnRzPSI2NDUuNyw0ODAuMyA2NjIuMyw0ODkuOSA2NjIuMyw1MDkgNjQ1LjcsNTE4LjYgNjI5LjIsNTA5IDYyOS4yLDQ4OS45IAkiLz4KCTxwb2x5Z29uIHN0cm9rZT0iI0ZGRkZGRiIgc3Ryb2tlLXdpZHRoPSIxLjUiIGZpbGw9IiM1NTAwRkYiIHBvaW50cz0iNjEyLjcsNDgwLjMgNjI5LjIsNDg5LjkgNjI5LjIsNTA5IDYxMi43LDUxOC42IDU5Ni4yLDUwOSA1OTYuMiw0ODkuOSAJIi8+Cgk8cG9seWdvbiBzdHJva2U9IiNGRkZGRkYiIHN0cm9rZS13aWR0aD0iMS41IiBmaWxsPSIjNTU1NTAwIiBwb2ludHM9IjYxMi43LDM2NS41IDYyOS4yLDM3NS4xIDYyOS4yLDM5NC4yIDYxMi43LDQwMy44IDU5Ni4yLDM5NC4yIDU5Ni4yLDM3NS4xIAkiLz4KCTxwb2x5Z29uIHN0cm9rZT0iI0ZGRkZGRiIgc3Ryb2tlLXdpZHRoPSIxLjUiIGZpbGw9IiM1NTU1NTUiIHBvaW50cz0iNzYxLjQsMjc5LjQgNzc3LjksMjg5IDc3Ny45LDMwOC4xIDc2MS40LDMxNy43IDc0NC45LDMwOC4xIDc0NC45LDI4OSAJIi8+Cgk8cG9seWdvbiBzdHJva2U9IiNGRkZGRkYiIHN0cm9rZS13aWR0aD0iMS41IiBmaWxsPSIjNTU1NUFBIiBwb2ludHM9IjYyOS4yLDUwOSA2NDUuNyw1MTguNiA2NDUuNyw1MzcuNyA2MjkuMiw1NDcuMiA2MTIuNyw1MzcuNyA2MTIuNyw1MTguNiAJIi8+Cgk8cG9seWdvbiBzdHJva2U9IiNGRkZGRkYiIHN0cm9rZS13aWR0aD0iMS41IiBmaWxsPSIjNTU1NUZGIiBwb2ludHM9IjU5Ni4yLDUwOSA2MTIuNyw1MTguNiA2MTIuNyw1MzcuNyA1OTYuMiw1NDcuMyA1NzkuNyw1MzcuNyA1NzkuNyw1MTguNiAJIi8+Cgk8cG9seWdvbiBzdHJva2U9IiNGRkZGRkYiIHN0cm9rZS13aWR0aD0iMS41IiBmaWxsPSIjNTVBQTAwIiBwb2ludHM9IjU2My4xLDMzNi45IDU3OS43LDM0Ni40IDU3OS43LDM2NS41IDU2My4xLDM3NS4xIDU0Ni42LDM2NS42IDU0Ni42LDM0Ni40IAkiLz4KCTxwb2x5Z29uIHN0cm9rZT0iI0ZGRkZGRiIgc3Ryb2tlLXdpZHRoPSIxLjUiIGZpbGw9IiM1NUFBNTUiIHBvaW50cz0iNTQ2LjYsMzY1LjUgNTYzLjEsMzc1LjEgNTYzLjEsMzk0LjIgNTQ2LjYsNDAzLjggNTMwLjEsMzk0LjIgNTMwLjEsMzc1LjEgCSIvPgoJPHBvbHlnb24gc3Ryb2tlPSIjRkZGRkZGIiBzdHJva2Utd2lkdGg9IjEuNSIgZmlsbD0iIzU1QUFBQSIgcG9pbnRzPSI1NjMuMSwzOTQuMiA1NzkuNyw0MDMuOCA1NzkuNyw0MjIuOSA1NjMuMSw0MzIuNSA1NDYuNiw0MjIuOSA1NDYuNiw0MDMuOCAJIi8+Cgk8cG9seWdvbiBzdHJva2U9IiNGRkZGRkYiIHN0cm9rZS13aWR0aD0iMS41IiBmaWxsPSIjNTVBQUZGIiBwb2ludHM9IjU0Ni42LDQ4MC4zIDU2My4xLDQ4OS45IDU2My4xLDUwOSA1NDYuNiw1MTguNiA1MzAuMSw1MDkgNTMwLjEsNDg5LjkgCSIvPgoJPHBvbHlnb24gc3Ryb2tlPSIjRkZGRkZGIiBzdHJva2Utd2lkdGg9IjEuNSIgZmlsbD0iIzU1RkYwMCIgcG9pbnRzPSI1MzAuMSwyNzkuNSA1NDYuNiwyODkgNTQ2LjYsMzA4LjIgNTMwLjEsMzE3LjcgNTEzLjYsMzA4LjIgNTEzLjYsMjg5IAkiLz4KCTxwb2x5Z29uIHN0cm9rZT0iI0ZGRkZGRiIgc3Ryb2tlLXdpZHRoPSIxLjUiIGZpbGw9IiM1NUZGNTUiIHBvaW50cz0iNDgwLjUsMzA4LjIgNDk3LDMxNy43IDQ5NywzMzYuOSA0ODAuNSwzNDYuNCA0NjQsMzM2LjkgNDY0LDMxNy43IAkiLz4KCTxwb2x5Z29uIHN0cm9rZT0iI0ZGRkZGRiIgc3Ryb2tlLXdpZHRoPSIxLjUiIGZpbGw9IiM1NUZGQUEiIHBvaW50cz0iNDgwLjUsMzY1LjYgNDk3LDM3NS4xIDQ5NywzOTQuMyA0ODAuNSw0MDMuOCA0NjQsMzk0LjMgNDY0LDM3NS4xIAkiLz4KCTxwb2x5Z29uIHN0cm9rZT0iI0ZGRkZGRiIgc3Ryb2tlLXdpZHRoPSIxLjUiIGZpbGw9IiM1NUZGRkYiIHBvaW50cz0iNDgwLjUsNDIzIDQ5Nyw0MzIuNSA0OTcsNDUxLjYgNDgwLjUsNDYxLjIgNDY0LDQ1MS43IDQ2NCw0MzIuNSAJIi8+Cgk8cG9seWdvbiBzdHJva2U9IiNGRkZGRkYiIHN0cm9rZS13aWR0aD0iMS41IiBmaWxsPSIjQUEwMDAwIiBwb2ludHM9IjcxMS44LDQyMi45IDcyOC40LDQzMi41IDcyOC40LDQ1MS42IDcxMS44LDQ2MS4yIDY5NS4zLDQ1MS42IDY5NS4zLDQzMi41IAkiLz4KCTxwb2x5Z29uIHN0cm9rZT0iI0ZGRkZGRiIgc3Ryb2tlLXdpZHRoPSIxLjUiIGZpbGw9IiNBQTAwNTUiIHBvaW50cz0iNjk1LjMsNDUxLjYgNzExLjgsNDYxLjIgNzExLjgsNDgwLjMgNjk1LjMsNDg5LjkgNjc4LjgsNDgwLjMgNjc4LjgsNDYxLjIgCSIvPgoJPHBvbHlnb24gc3Ryb2tlPSIjRkZGRkZGIiBzdHJva2Utd2lkdGg9IjEuNSIgZmlsbD0iI0FBMDBBQSIgcG9pbnRzPSI2NzguOCw0ODAuMyA2OTUuMyw0ODkuOSA2OTUuMyw1MDkgNjc4LjgsNTE4LjYgNjYyLjMsNTA5IDY2Mi4zLDQ4OS45IAkiLz4KCTxwb2x5Z29uIHN0cm9rZT0iI0ZGRkZGRiIgc3Ryb2tlLXdpZHRoPSIxLjUiIGZpbGw9IiNBQTAwRkYiIHBvaW50cz0iNjYyLjMsNTA5IDY3OC44LDUxOC41IDY3OC44LDUzNy43IDY2Mi4zLDU0Ny4yIDY0NS43LDUzNy43IDY0NS43LDUxOC41IAkiLz4KCTxwb2x5Z29uIHN0cm9rZT0iI0ZGRkZGRiIgc3Ryb2tlLXdpZHRoPSIxLjUiIGZpbGw9IiNBQTU1MDAiIHBvaW50cz0iNjc4LjgsMzY1LjUgNjk1LjMsMzc1LjEgNjk1LjMsMzk0LjIgNjc4LjgsNDAzLjggNjYyLjMsMzk0LjIgNjYyLjMsMzc1LjEgCSIvPgoJPHBvbHlnb24gc3Ryb2tlPSIjRkZGRkZGIiBzdHJva2Utd2lkdGg9IjEuNSIgZmlsbD0iI0FBNTU1NSIgcG9pbnRzPSI2OTUuMywzOTQuMiA3MTEuOCw0MDMuOCA3MTEuOCw0MjIuOSA2OTUuMyw0MzIuNSA2NzguOCw0MjIuOSA2NzguOCw0MDMuOCAJIi8+Cgk8cG9seWdvbiBzdHJva2U9IiNGRkZGRkYiIHN0cm9rZS13aWR0aD0iMS41IiBmaWxsPSIjQUE1NUFBIiBwb2ludHM9IjY5NS4zLDUwOSA3MTEuOCw1MTguNSA3MTEuOCw1MzcuNiA2OTUuMyw1NDcuMiA2NzguOCw1MzcuNyA2NzguOCw1MTguNSAJIi8+Cgk8cG9seWdvbiBzdHJva2U9IiNGRkZGRkYiIHN0cm9rZS13aWR0aD0iMS41IiBmaWxsPSIjQUE1NUZGIiBwb2ludHM9IjY3OC44LDUzNy43IDY5NS4zLDU0Ny4yIDY5NS4zLDU2Ni40IDY3OC44LDU3NS45IDY2Mi4zLDU2Ni40IDY2Mi4zLDU0Ny4yIAkiLz4KCTxwb2x5Z29uIHN0cm9rZT0iI0ZGRkZGRiIgc3Ryb2tlLXdpZHRoPSIxLjUiIGZpbGw9IiNBQUFBMDAiIHBvaW50cz0iNjI5LjIsMzM2LjggNjQ1LjcsMzQ2LjQgNjQ1LjcsMzY1LjUgNjI5LjIsMzc1LjEgNjEyLjcsMzY1LjUgNjEyLjcsMzQ2LjQgCSIvPgoJPHBvbHlnb24gc3Ryb2tlPSIjRkZGRkZGIiBzdHJva2Utd2lkdGg9IjEuNSIgZmlsbD0iI0FBQUE1NSIgcG9pbnRzPSI1OTYuMiwzMzYuOSA2MTIuNywzNDYuNCA2MTIuNywzNjUuNSA1OTYuMiwzNzUuMSA1NzkuNywzNjUuNiA1NzkuNywzNDYuNCAJIi8+Cgk8cG9seWdvbiBzdHJva2U9IiNGRkZGRkYiIHN0cm9rZS13aWR0aD0iMS41IiBmaWxsPSIjQUFBQUFBIiBwb2ludHM9IjcxMS44LDI1MC43IDcyOC40LDI2MC4zIDcyOC40LDI3OS40IDcxMS44LDI4OSA2OTUuMywyNzkuNCA2OTUuMywyNjAuMyAJIi8+Cgk8cG9seWdvbiBzdHJva2U9IiNGRkZGRkYiIHN0cm9rZS13aWR0aD0iMS41IiBmaWxsPSIjRkZGRkZGIiBwb2ludHM9Ijc0NC45LDI1MC43IDc2MS40LDI2MC4zIDc2MS40LDI3OS40IDc0NC45LDI4OSA3MjguNCwyNzkuNCA3MjguNCwyNjAuMyAJIi8+Cgk8cG9seWdvbiBzdHJva2U9IiNGRkZGRkYiIHN0cm9rZS13aWR0aD0iMS41IiBmaWxsPSIjQUFBQUZGIiBwb2ludHM9IjYxMi43LDUzNy43IDYyOS4yLDU0Ny4zIDYyOS4yLDU2Ni40IDYxMi43LDU3NS45IDU5Ni4yLDU2Ni40IDU5Ni4yLDU0Ny4zIAkiLz4KCTxwb2x5Z29uIHN0cm9rZT0iI0ZGRkZGRiIgc3Ryb2tlLXdpZHRoPSIxLjUiIGZpbGw9IiNBQUZGMDAiIHBvaW50cz0iNTYzLjEsMjc5LjUgNTc5LjcsMjg5IDU3OS43LDMwOC4xIDU2My4xLDMxNy43IDU0Ni42LDMwOC4yIDU0Ni42LDI4OSAJIi8+Cgk8cG9seWdvbiBzdHJva2U9IiNGRkZGRkYiIHN0cm9rZS13aWR0aD0iMS41IiBmaWxsPSIjQUFGRjU1IiBwb2ludHM9IjU0Ni42LDI1MC44IDU2My4xLDI2MC4zIDU2My4xLDI3OS41IDU0Ni42LDI4OSA1MzAuMSwyNzkuNSA1MzAuMSwyNjAuMyAJIi8+Cgk8cG9seWdvbiBzdHJva2U9IiNGRkZGRkYiIHN0cm9rZS13aWR0aD0iMS41IiBmaWxsPSIjQUFGRkFBIiBwb2ludHM9IjQ2NCwyNzkuNSA0ODAuNSwyODkgNDgwLjUsMzA4LjIgNDY0LDMxNy43IDQ0Ny41LDMwOC4yIDQ0Ny41LDI4OSAJIi8+Cgk8cG9seWdvbiBzdHJva2U9IiNGRkZGRkYiIHN0cm9rZS13aWR0aD0iMS41IiBmaWxsPSIjQUFGRkZGIiBwb2ludHM9IjQ2NCw0NTEuNyA0ODAuNSw0NjEuMiA0ODAuNSw0ODAuNCA0NjQsNDg5LjkgNDQ3LjUsNDgwLjQgNDQ3LjUsNDYxLjIgCSIvPgoJPHBvbHlnb24gc3Ryb2tlPSIjRkZGRkZGIiBzdHJva2Utd2lkdGg9IjEuNSIgZmlsbD0iI0ZGMDAwMCIgcG9pbnRzPSI3MTEuOCwzNjUuNSA3MjguNCwzNzUuMSA3MjguNCwzOTQuMiA3MTEuOCw0MDMuOCA2OTUuMywzOTQuMiA2OTUuMywzNzUuMSAJIi8+Cgk8cG9seWdvbiBzdHJva2U9IiNGRkZGRkYiIHN0cm9rZS13aWR0aD0iMS41IiBmaWxsPSIjRkYwMDU1IiBwb2ludHM9IjcyOC40LDM5NC4yIDc0NC45LDQwMy44IDc0NC45LDQyMi45IDcyOC40LDQzMi40IDcxMS44LDQyMi45IDcxMS44LDQwMy44IAkiLz4KCTxwb2x5Z29uIHN0cm9rZT0iI0ZGRkZGRiIgc3Ryb2tlLXdpZHRoPSIxLjUiIGZpbGw9IiNGRjAwQUEiIHBvaW50cz0iNzI4LjQsNDUxLjYgNzQ0LjksNDYxLjIgNzQ0LjksNDgwLjMgNzI4LjQsNDg5LjkgNzExLjgsNDgwLjMgNzExLjgsNDYxLjIgCSIvPgoJPHBvbHlnb24gc3Ryb2tlPSIjRkZGRkZGIiBzdHJva2Utd2lkdGg9IjEuNSIgZmlsbD0iI0ZGMDBGRiIgcG9pbnRzPSI3MTEuOCw0ODAuMyA3MjguNCw0ODkuOSA3MjguNCw1MDkgNzExLjgsNTE4LjUgNjk1LjMsNTA5IDY5NS4zLDQ4OS45IAkiLz4KCTxwb2x5Z29uIHN0cm9rZT0iI0ZGRkZGRiIgc3Ryb2tlLXdpZHRoPSIxLjUiIGZpbGw9IiNGRjU1MDAiIHBvaW50cz0iNjk1LjMsMzM2LjggNzExLjgsMzQ2LjQgNzExLjgsMzY1LjUgNjk1LjMsMzc1LjEgNjc4LjgsMzY1LjUgNjc4LjgsMzQ2LjQgCSIvPgoJPHBvbHlnb24gc3Ryb2tlPSIjRkZGRkZGIiBzdHJva2Utd2lkdGg9IjEuNSIgZmlsbD0iI0ZGNTU1NSIgcG9pbnRzPSI3NDQuOSwzNjUuNSA3NjEuNCwzNzUuMSA3NjEuNCwzOTQuMiA3NDQuOSw0MDMuNyA3MjguNCwzOTQuMiA3MjguNCwzNzUuMSAJIi8+Cgk8cG9seWdvbiBzdHJva2U9IiNGRkZGRkYiIHN0cm9rZS13aWR0aD0iMS41IiBmaWxsPSIjRkY1NUFBIiBwb2ludHM9Ijc0NC45LDQ4MC4zIDc2MS40LDQ4OS44IDc2MS40LDUwOSA3NDQuOSw1MTguNSA3MjguNCw1MDkgNzI4LjQsNDg5LjggCSIvPgoJPHBvbHlnb24gc3Ryb2tlPSIjRkZGRkZGIiBzdHJva2Utd2lkdGg9IjEuNSIgZmlsbD0iI0ZGNTVGRiIgcG9pbnRzPSI3MjguNCw1MDkgNzQ0LjksNTE4LjYgNzQ0LjksNTM3LjcgNzI4LjQsNTQ3LjIgNzExLjgsNTM3LjcgNzExLjgsNTE4LjYgCSIvPgoJPHBvbHlnb24gc3Ryb2tlPSIjRkZGRkZGIiBzdHJva2Utd2lkdGg9IjEuNSIgZmlsbD0iI0ZGQUEwMCIgcG9pbnRzPSI2NjIuMywzMzYuOCA2NzguOCwzNDYuNCA2NzguOCwzNjUuNSA2NjIuMywzNzUuMSA2NDUuNywzNjUuNSA2NDUuNywzNDYuNCAJIi8+Cgk8cG9seWdvbiBzdHJva2U9IiNGRkZGRkYiIHN0cm9rZS13aWR0aD0iMS41IiBmaWxsPSIjRkZBQTU1IiBwb2ludHM9IjY3OC44LDMwOC4xIDY5NS4zLDMxNy43IDY5NS4zLDMzNi44IDY3OC44LDM0Ni40IDY2Mi4zLDMzNi44IDY2Mi4zLDMxNy43IAkiLz4KCTxwb2x5Z29uIHN0cm9rZT0iI0ZGRkZGRiIgc3Ryb2tlLXdpZHRoPSIxLjUiIGZpbGw9IiNGRkFBQUEiIHBvaW50cz0iNzYxLjQsMzM2LjggNzc3LjksMzQ2LjQgNzc3LjksMzY1LjUgNzYxLjQsMzc1LjEgNzQ0LjksMzY1LjUgNzQ0LjksMzQ2LjQgCSIvPgoJPHBvbHlnb24gc3Ryb2tlPSIjRkZGRkZGIiBzdHJva2Utd2lkdGg9IjEuNSIgZmlsbD0iI0ZGQUFGRiIgcG9pbnRzPSI3NjEuNCw1MDkgNzc3LjksNTE4LjUgNzc3LjksNTM3LjYgNzYxLjQsNTQ3LjIgNzQ0LjksNTM3LjcgNzQ0LjksNTE4LjUgCSIvPgoJPHBvbHlnb24gc3Ryb2tlPSIjRkZGRkZGIiBzdHJva2Utd2lkdGg9IjEuNSIgZmlsbD0iI0ZGRkYwMCIgcG9pbnRzPSI2NDUuNywzMDguMSA2NjIuMywzMTcuNyA2NjIuMywzMzYuOCA2NDUuNywzNDYuNCA2MjkuMiwzMzYuOCA2MjkuMiwzMTcuNyAJIi8+Cgk8cG9seWdvbiBzdHJva2U9IiNGRkZGRkYiIHN0cm9rZS13aWR0aD0iMS41IiBmaWxsPSIjRkZGRjU1IiBwb2ludHM9IjY2Mi4zLDI3OS40IDY3OC44LDI4OSA2NzguOCwzMDguMSA2NjIuMywzMTcuNyA2NDUuNywzMDguMSA2NDUuNywyODkgCSIvPgoJPHBvbHlnb24gc3Ryb2tlPSIjRkZGRkZGIiBzdHJva2Utd2lkdGg9IjEuNSIgZmlsbD0iI0ZGRkZBQSIgcG9pbnRzPSI2NDUuNywyNTAuNyA2NjIuMywyNjAuMyA2NjIuMywyNzkuNCA2NDUuNywyODkgNjI5LjIsMjc5LjQgNjI5LjIsMjYwLjMgCSIvPgo8L3N2Zz4=)

## Color Information

Above the table, two buttons re-render the color map:

- **Uncorrected** - shows every hexagon in its native color.
- **Sunlight** - shows how each color appears in direct sunlight, using the mapping in
  "The Sunlight Color Map" below.

After a hexagon is clicked, the table shows:

| Field: | Contents: |
| ------ | --------- |
| Name: | The color's name, linking to its Wikipedia reference page. |
| Sample: | A swatch rendered in the selected (corrected) color; White gets a 1px black border. |
| HTML code: | The corrected HTML code (e.g. `#001E41` for Duke Blue under the Sunlight map). |
| Uncorrected HTML code: | The uncorrected HTML code (e.g. `#000055`). |
| SDK Constant: | The color's SDK constant (e.g. `GColorDukeBlue`). |
| Code (RGB): | e.g. `GColorFromRGB(0, 85, 255)`. |
| Code (Hex): | e.g. `GColorFromHEX(0x000055)`. |

The URL fragment is also set to the selected hex (e.g. `#000055`), so a selection can be
linked or reloaded directly.

## Example Code Segment

When a color is selected, the tool displays this sample with the selected color's SDK
constant substituted (shown here for Mint Green):

```c
Window *window = window_create();
window_set_background_color(window, GColorMintGreen);
window_stack_push(window, true);
```

See the [*Images*](https://developer.repebble.com/guides/app-resources/images/#color-palettes)
guide to get compatible palette files for popular image editing programs.

## The Sunlight Color Map

The Sunlight button re-renders the color map using this mapping, transcribed from the
page's own `color-mapping-sunlight.js` (uncorrected -> corrected hex, one entry per
color, in hex order):

```sh
# uncorrected -> corrected
  #000000 -> #000000  # Black
  #000055 -> #001E41  # Oxford Blue
  #0000AA -> #004387  # Duke Blue
  #0000FF -> #0068CA  # Blue
  #005500 -> #2B4A2C  # Dark Green (X11)
  #005555 -> #27514F  # Midnight Green (Eagle Green)
  #0055AA -> #16638D  # Cobalt Blue
  #0055FF -> #007DCE  # Blue Moon
  #00AA00 -> #5E9860  # Islamic Green
  #00AA55 -> #5C9B72  # Jaeger Green
  #00AAAA -> #57A5A2  # Tiffany Blue
  #00AAFF -> #4CB4DB  # Vivid Cerulean
  #00FF00 -> #8EE391  # Green
  #00FF55 -> #8EE69E  # Malachite
  #00FFAA -> #8AEBC0  # Medium Spring Green
  #00FFFF -> #84F5F1  # Cyan
  #550000 -> #4A161B  # Bulgarian Rose
  #550055 -> #482748  # Imperial Purple
  #5500AA -> #40488A  # Indigo (Web)
  #5500FF -> #2F6BCC  # Electric Ultramarine
  #555500 -> #564E36  # Army Green
  #555555 -> #545454  # Dark Gray
  #5555AA -> #4F6790  # Liberty
  #5555FF -> #4180D0  # Very Light Blue
  #55AA00 -> #759A64  # Kelly Green
  #55AA55 -> #759D76  # May Green
  #55AAAA -> #71A6A4  # Cadet Blue
  #55AAFF -> #69B5DD  # Picton Blue
  #55FF00 -> #9EE594  # Bright Green
  #55FF55 -> #9DE7A0  # Screamin' Green
  #55FFAA -> #9BECC2  # Medium Aquamarine
  #55FFFF -> #95F6F2  # Electric Blue
  #AA0000 -> #99353F  # Dark Candy Apple Red
  #AA0055 -> #983E5A  # Jazzberry Jam
  #AA00AA -> #955694  # Purple
  #AA00FF -> #8F74D2  # Vivid Violet
  #AA5500 -> #9D5B4D  # Windsor Tan
  #AA5555 -> #9D6064  # Rose Vale
  #AA55AA -> #9A7099  # Purpureus
  #AA55FF -> #9587D5  # Lavender Indigo
  #AAAA00 -> #AFA072  # Limerick
  #AAAA55 -> #AEA382  # Brass
  #AAAAAA -> #ABABAB  # Light Gray
  #AAAAFF -> #A7BAE2  # Baby Blue Eyes
  #AAFF00 -> #C9E89D  # Spring Bud
  #AAFF55 -> #C9EAA7  # Inchworm
  #AAFFAA -> #C7F0C8  # Mint Green
  #AAFFFF -> #C3F9F7  # Celeste
  #FF0000 -> #E35462  # Red
  #FF0055 -> #E25874  # Folly
  #FF00AA -> #E16AA3  # Fashion Magenta
  #FF00FF -> #DE83DC  # Magenta
  #FF5500 -> #E66E6B  # Orange
  #FF5555 -> #E6727C  # Sunset Orange
  #FF55AA -> #E37FA7  # Brilliant Rose
  #FF55FF -> #E194DF  # Shocking Pink (Crayola)
  #FFAA00 -> #F1AA86  # Chrome Yellow
  #FFAA55 -> #F1AD93  # Rajah
  #FFAAAA -> #EFB5B8  # Melon
  #FFAAFF -> #ECC3EB  # Rich Brilliant Lavender
  #FFFF00 -> #FFEEAB  # Yellow
  #FFFF55 -> #FFF1B5  # Icterine
  #FFFFAA -> #FFF6D3  # Pastel Yellow
  #FFFFFF -> #FFFFFF  # White
```

## All Colors (complete list)

Every color the tool offers, transcribed from `color-dict.js` (the page's own data
file), sorted by hex code: hexagon fill (uncorrected HTML color), name (linking to the
Wikipedia reference the page itself stores), and SDK constant.

| Hex | Name | SDK Constant |
| --- | ---- | ------------ |
| #000000 | [Black](http://en.wikipedia.org/wiki/Black) | GColorBlack |
| #000055 | [Oxford Blue](http://en.wikipedia.org/wiki/Oxford_University) | GColorOxfordBlue |
| #0000AA | [Duke Blue](http://en.wikipedia.org/wiki/Duke_blue) | GColorDukeBlue |
| #0000FF | [Blue](http://en.wikipedia.org/wiki/Blue) | GColorBlue |
| #005500 | [Dark Green (X11)](http://en.wikipedia.org/wiki/Shades_of_green#Dark_green_.28X11.29) | GColorDarkGreen |
| #005555 | [Midnight Green (Eagle Green)](http://en.wikipedia.org/wiki/Shades_of_green#Midnight_green) | GColorMidnightGreen |
| #0055AA | [Cobalt Blue](http://en.wikipedia.org/wiki/Cobalt_blue) | GColorCobaltBlue |
| #0055FF | [Blue Moon](http://en.wikipedia.org/wiki/Blue_Moon_%28beer%29) | GColorBlueMoon |
| #00AA00 | [Islamic Green](http://en.wikipedia.org/wiki/Shades_of_green#Islamic_green) | GColorIslamicGreen |
| #00AA55 | [Jaeger Green](http://en.wikipedia.org/wiki/Jägermeister) | GColorJaegerGreen |
| #00AAAA | [Tiffany Blue](http://en.wikipedia.org/wiki/Tiffany_Blue#Tiffany_Blue) | GColorTiffanyBlue |
| #00AAFF | [Vivid Cerulean](http://en.wikipedia.org/wiki/Cerulean) | GColorVividCerulean |
| #00FF00 | [Green](http://en.wikipedia.org/wiki/Green) | GColorGreen |
| #00FF55 | [Malachite](http://en.wikipedia.org/wiki/Shades_of_green#Malachite) | GColorMalachite |
| #00FFAA | [Medium Spring Green](http://en.wikipedia.org/wiki/Spring_green_%28color%29#Medium_spring_green) | GColorMediumSpringGreen |
| #00FFFF | [Cyan](http://en.wikipedia.org/wiki/Cyan) | GColorCyan |
| #550000 | [Bulgarian Rose](http://en.wikipedia.org/wiki/Rose_%28color%29#Bulgarian_Rose) | GColorBulgarianRose |
| #550055 | [Imperial Purple](http://en.wikipedia.org/wiki/Purple#Tyrian_purple:_Classical_antiquity) | GColorImperialPurple |
| #5500AA | [Indigo (Web)](http://en.wikipedia.org/wiki/Indigo#Pigment_indigo_.28web_color_indigo.29) | GColorIndigo |
| #5500FF | [Electric Ultramarine](http://en.wikipedia.org/wiki/Ultramarine#Electric_ultramarine) | GColorElectricUltramarine |
| #555500 | [Army Green](http://en.wikipedia.org/wiki/Army_green) | GColorArmyGreen |
| #555555 | [Dark Gray](http://en.wikipedia.org/wiki/Shades_of_gray#Dark_medium_gray_.28dark_gray_.28X11.29.29) | GColorDarkGray |
| #5555AA | [Liberty](http://en.wikipedia.org/wiki/Variations_of_blue#Liberty) | GColorLiberty |
| #5555FF | [Very Light Blue](http://en.wikipedia.org/wiki/Light_blue) | GColorVeryLightBlue |
| #55AA00 | [Kelly Green](http://en.wikipedia.org/wiki/Shades_of_green#Kelly_green) | GColorKellyGreen |
| #55AA55 | [May Green](http://en.wikipedia.org/wiki/Spring_bud#May_green) | GColorMayGreen |
| #55AAAA | [Cadet Blue](http://en.wikipedia.org/wiki/Cadet_grey#Cadet_blue) | GColorCadetBlue |
| #55AAFF | [Picton Blue](http://en.wikipedia.org/wiki/Azure_%28color%29#Picton_blue) | GColorPictonBlue |
| #55FF00 | [Bright Green](http://en.wikipedia.org/wiki/Shades_of_green#Bright_green) | GColorBrightGreen |
| #55FF55 | [Screamin' Green](http://en.wikipedia.org/wiki/List_of_Crayola_crayon_colors#Standard_colors) | GColorScreaminGreen |
| #55FFAA | [Medium Aquamarine](http://en.wikipedia.org/wiki/Aquamarine_%28color%29#Medium_aquamarine) | GColorMediumAquamarine |
| #55FFFF | [Electric Blue](http://en.wikipedia.org/wiki/Electric_blue_%28color%29) | GColorElectricBlue |
| #AA0000 | [Dark Candy Apple Red](http://en.wikipedia.org/wiki/Candy_apple_red_%28color%29#Dark_candy_apple_red) | GColorDarkCandyAppleRed |
| #AA0055 | [Jazzberry Jam](http://en.wikipedia.org/wiki/Red-violet#Jazzberry_jam) | GColorJazzberryJam |
| #AA00AA | [Purple](http://en.wikipedia.org/wiki/Purple) | GColorPurple |
| #AA00FF | [Vivid Violet](http://en.wikipedia.org/wiki/Shades_of_violet#Vivid_violet) | GColorVividViolet |
| #AA5500 | [Windsor Tan](http://en.wikipedia.org/wiki/Tan_%28color%29#Windsor_tan) | GColorWindsorTan |
| #AA5555 | [Rose Vale](http://en.wikipedia.org/wiki/Rose_%28color%29#Rose_vale) | GColorRoseVale |
| #AA55AA | [Purpureus](http://en.wikipedia.org/wiki/Shades_of_purple#Purpureus) | GColorPurpureus |
| #AA55FF | [Lavender Indigo](http://en.wikipedia.org/wiki/Lavender_%28color%29#Lavender_indigo) | GColorLavenderIndigo |
| #AAAA00 | [Limerick](http://en.wikipedia.org/wiki/Chartreuse_%28color%29#Limerick) | GColorLimerick |
| #AAAA55 | [Brass](http://en.wikipedia.org/wiki/Brass) | GColorBrass |
| #AAAAAA | [Light Gray](http://en.wikipedia.org/wiki/Shades_of_gray#Light_gray) | GColorLightGray |
| #AAAAFF | [Baby Blue Eyes](http://en.wikipedia.org/wiki/Baby_blue#Baby_blue_eyes) | GColorBabyBlueEyes |
| #AAFF00 | [Spring Bud](http://en.wikipedia.org/wiki/Spring_bud) | GColorSpringBud |
| #AAFF55 | [Inchworm](http://en.wikipedia.org/wiki/List_of_Crayola_crayon_colors#Standard_colors) | GColorInchworm |
| #AAFFAA | [Mint Green](http://en.wikipedia.org/wiki/Spring_green_%28color%29#Mint_green) | GColorMintGreen |
| #AAFFFF | [Celeste](http://en.wikipedia.org/wiki/Sky_Blue#Celeste) | GColorCeleste |
| #FF0000 | [Red](http://en.wikipedia.org/wiki/Red) | GColorRed |
| #FF0055 | [Folly](http://en.wikipedia.org/wiki/Crimson#Folly) | GColorFolly |
| #FF00AA | [Fashion Magenta](http://en.wikipedia.org/wiki/Fuchsia_%28color%29#Fashion_fuchsia) | GColorFashionMagenta |
| #FF00FF | [Magenta](http://en.wikipedia.org/wiki/Magenta) | GColorMagenta |
| #FF5500 | [Orange](http://en.wikipedia.org/wiki/Orange_%28colour%29) | GColorOrange |
| #FF5555 | [Sunset Orange](http://en.wikipedia.org/wiki/Sunset_%28color%29#Sunset_orange) | GColorSunsetOrange |
| #FF55AA | [Brilliant Rose](http://en.wikipedia.org/wiki/Rose_%28color%29#Brilliant_rose) | GColorBrilliantRose |
| #FF55FF | [Shocking Pink (Crayola)](http://en.wikipedia.org/wiki/Variations_of_magenta#Ultra_pink) | GColorShockingPink |
| #FFAA00 | [Chrome Yellow](http://en.wikipedia.org/wiki/Chrome_yellow) | GColorChromeYellow |
| #FFAA55 | [Rajah](http://en.wikipedia.org/wiki/Saffron_%28color%29#Rajah) | GColorRajah |
| #FFAAAA | [Melon](http://en.wikipedia.org/wiki/Variations_of_orange#Melon) | GColorMelon |
| #FFAAFF | [Rich Brilliant Lavender](http://en.wikipedia.org/wiki/Lavender_%28color%29#Rich_brilliant_lavender) | GColorRichBrilliantLavender |
| #FFFF00 | [Yellow](http://en.wikipedia.org/wiki/Yellow) | GColorYellow |
| #FFFF55 | [Icterine](http://en.wikipedia.org/wiki/Icterine) | GColorIcterine |
| #FFFFAA | [Pastel Yellow](http://en.wikipedia.org/wiki/Shades_of_yellow#Pastel_yellow) | GColorPastelYellow |
| #FFFFFF | [White](http://en.wikipedia.org/wiki/White) | GColorWhite |

Each color's name links to the reference URL the page itself stores for it (the page URL,
all asset URLs and all 63 unique reference URLs above were HTTP-checked: every one
returns 200, and every unique referenced Wikipedia page exists).

## SDK supplement (not from the page)

This section is derived from the local Pebble SDK, not the page. The SDK is installed at
`/Users/cmalec/Library/Application Support/Pebble SDK/SDKs/current/sdk-core/pebble/`.

- The header `<platform>/include/gcolor_definitions.h` (byte-identical - same MD5 - for
  aplite, basalt, chalk, diorite, emery, flint and gabbro) defines a named color macro
  for all 64 picker colors. Every SDK constant name in the table above and every ARGB8
  bit pattern in `color-dict.js` matches that header exactly.
- The header additionally defines one color absent from the picker:

```c
// Additional 8bit color values
#define GColorClearARGB8 ((uint8_t)0b00000000)

// Additional GColor values
#define GColorClear ((GColor8){.argb=GColorClearARGB8})
```

- `GColorFromRGB`, `GColorFromHEX` and `GColorFromRGBA` (the conversion macros the
  table's "Code (RGB)" and "Code (Hex)" literals rely on) are defined in the same
  header:

```c
#define GColorFromRGBA(red, green, blue, alpha) ((GColor8){ \
  .b = (uint8_t)(blue) >> 6, \
  .g = (uint8_t)(green) >> 6, \
  .r = (uint8_t)(red) >> 6, \
  .a = (uint8_t)(alpha) >> 6, \
  })

#define GColorFromRGB(red, green, blue) \
  GColorFromRGBA(red, green, blue, 255)

#define GColorFromHEX(v) GColorFromRGB(((v) >> 16) & 0xff, ((v) >> 8) & 0xff, ((v) & 0xff))
```
