# Shared contracts

This directory owns interfaces that are shared by independently developed project components.

## Ownership rule

A shared contract is owned by the product/integration repository, not by one implementation side. `mouse-ui` and `mouse-core` may propose changes, but neither silently changes the public contract by changing private code.

## Current contracts

- [`ui-core/`](ui-core/README.md) — semantic boundary between frontend UX and backend Core.

## Stability rule

Drafts may change freely. A released contract is immutable in meaning; incompatible changes require a new version.
