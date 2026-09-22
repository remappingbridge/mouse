# UIC-04 capability, limit and error fixtures

These JSON fixtures exercise the semantic rules in
`contracts/ui-core/drafts/uic-04-capabilities-limits-errors.md`.

JSON is test notation only. It is not the final descriptor/error C ABI.

The fixtures prove:

- v1 compatibility discovery without backend technology leakage;
- the public 16-saved-Mouse limit;
- the 63-byte valid UTF-8 semantic name boundary;
- stable submission/result error categories;
- timeout remains a distinct terminal state rather than a synthetic error;
- provider/vendor diagnostics remain opaque and non-normative.
