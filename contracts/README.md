# Shared contracts

This directory owns interfaces shared by independently developed product components.

## Ownership rule

A shared contract is owned by the product/integration repository, not by one implementation side. `mouse-ui` and `mouse-core` may propose changes, but neither silently changes the public contract by changing private code.

## Current contracts

- [`ui-core/`](ui-core/README.md) — semantic boundary between frontend UX and backend Core.

## Current source baseline

The active product input is frozen `mouse-ui` **UI Layout 1.0** at `e8adad7919e931c92515bf655ef4050876a8e7a9` / `release/ui-layout-v1.0`.

## Stability rule

Drafts may change freely. A released contract is immutable in meaning; incompatible changes require a new version. A UI layout version and a contract version are independent version spaces.