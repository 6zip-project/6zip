6Zip Core staging tree
===========================

|CI|master|develop|
|-|-|-|
|Gitlab|[![Build Status](https://gitlab.com/6zip-online/6zip/badges/master/pipeline.svg)](https://gitlab.com/6zip-online/zip/-/tree/master)|[![Build Status](https://gitlab.com/6zip-online/zip/badges/develop/pipeline.svg)](https://gitlab.com/6zip-online/6zip/-/tree/master)|

https://6zip.online

For an immediately usable, binary version of the 6Zip Core software, see
https://6zip.online/downloads/.

Further information about 6Zip Core is available in the [doc folder](/doc).

What is 6Zip?
-------------

6ZIP is an experimental digital project that enables instant, private
payments to anyone, anywhere in the world and it is the next-generation blockchain project 
designed to bring unprecedented flexibility, security, and economic stability to the world of cryptocurrency. 
Our mission is to create a blockchain network that not only meets the demands of today but also adapts to the evolving needs 
of the future. At the heart of 6ZIP lies our revolutionary reward system and the unique HashX7 hashing 
algorithm, which set us apart from traditional blockchain models.

For more information read the original 6Zip whitepaper.

License
-------

6Zip Core is released under the terms of the MIT license. See [COPYING](COPYING) for more
information or see https://opensource.org/licenses/MIT.

Development Process
-------------------

The `master` branch is meant to be stable. Development is normally done in separate branches.
[Tags](https://github.com/6zip-online/6zip/tags) are created to indicate new official,
stable release versions of 6Zip Core.

The `develop` branch is regularly built (see doc/build-*.md for instructions) and tested, but is not guaranteed to be
completely stable.

The contribution workflow is described in [CONTRIBUTING.md](CONTRIBUTING.md)
and useful hints for developers can be found in [doc/developer-notes.md](doc/developer-notes.md).

Testing
-------

Testing and code review is the bottleneck for development; we get more pull
requests than we can review and test on short notice. Please be patient and help out by testing
other people's pull requests, and remember this is a security-critical project where any mistake might cost people
lots of money.

### Automated Testing

Developers are strongly encouraged to write [unit tests](src/test/README.md) for new code, and to
submit new unit tests for old code. Unit tests can be compiled and run
(assuming they weren't disabled in configure) with: `make check`. Further details on running
and extending unit tests can be found in [/src/test/README.md](/src/test/README.md).

There are also [regression and integration tests](/test), written
in Python.
These tests can be run (if the [test dependencies](/test) are installed) with: `test/functional/test_runner.py`

The CI (Continuous Integration) systems make sure that every pull request is built for Windows, Linux, and macOS,
and that unit/sanity tests are run automatically.

### Manual Quality Assurance (QA) Testing

Changes should be tested by somebody other than the developer who wrote the
code. This is especially important for large or high-risk changes. It is useful
to add a test plan to the pull request description if testing the changes is
not straightforward.

Translations
------------

Changes to translations as well as new translations can be submitted to
[6Zip Core's Transifex page](https://www.transifex.com/projects/p/6zip/).

Translations are periodically pulled from Transifex and merged into the git repository. See the
[translation process](doc/translation_process.md) for details on how this works.

**Important**: We do not accept translation changes as GitHub pull requests because the next
pull from Transifex would automatically overwrite them again.
