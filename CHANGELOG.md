# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project comply partially with [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

Breaking changes in code that is not part of [atomic-APIs](doc/api/vsync/atomic/README.md)
is not guaranteed to result in increment of major version.

Please note that the version correlates to the internal libvsync, which is a superset of
what exists in open-s4c libvsync.


### [3.6.0]

### Added

- cnalock, clhlock, arraylock, twalock, hmcslock, hclhlock, rec_seqlock and hemlock
- bitmap
- simpleht hashtable
- elimination and xbo backoff stacks
- mutex, cond, and once
- treeset

### Fixed

- implementation of `rwlock_acquired_by_readers`, which used to return
true also in the case of neither a writer nor a reader acquired the lock.
Now it returns true if and only if the lock is acquired by readers, as the name suggests.


### [3.5.0]

### Added

- vmm/vmm.cat and verification support

### [3.4.0]

### Changed

- Minor documentation updates

## [3.3.1]

### Changed

- documentation and minor updates

## [3.2.0] - 2024-01-26

Initial public release.
