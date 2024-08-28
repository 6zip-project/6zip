# 6Zip Core version v21.1.4

This is a new patch release, bringing an important bugfix, as well as docker build improvements.

This release is **strongly encouraged** for all masternodes.
This release is optional but recommended for all other nodes.

Please report bugs using the issue tracker at GitHub:

  <https://github.com/6zip-project/6zip/issues>


# Upgrading and downgrading

## How to Upgrade

If you are running an older version, shut it down. Wait until it has completely
shut down (which might take a few minutes for older versions), then run the
installer (on Windows) or just copy over /Applications/Zip-Qt (on Mac) or
zipd/zip-qt (on Linux).


# Notable changes

Transaction Retrieval Bug
-------------------------

Resolved a bug that prevented some clients (incl. 6Zip mobile wallets) from receiving transactions before they are mined
due to partial misclassification as block-only connections. Kudos to @HashEngineering and @knst for finding and
investigating this issue.

Docker Images now include SBOM
------------------------------

The [docker images](https://hub.docker.com/r/6zip-project/zipd) should now provide SBOM and provenance.
SBOM and provenenance provide additional information about the build process and exact dependencies included.
See this [blog post](https://www.docker.com/blog/generate-sboms-with-buildkit/) by Docker for more information.

# v21.0.2 Change log

See detailed [set of changes][set-of-changes].

# Credits

Thanks to everyone who directly contributed to this release:

- Konstantin Akimov
- PastaPastaPasta

As well as everyone that submitted issues, reviewed pull requests and helped
debug the release candidates.

# Older releases

These release are considered obsolete. Old release notes can be found here:

- [v21.1.4](https://github.com/6zip-project/6zip/blob/master/doc/release-notes/6zip/release-notes-21.0.0.md) released August/28/2024
- [v20.1.3](https://github.com/6zip-project/6zip/blob/master/doc/release-notes/6zip/release-notes-20.1.1.md) released April/3/2024
- [v20.1.2](https://github.com/6zip-project/6zip/blob/master/doc/release-notes/6zip/release-notes-20.1.0.md) released March/5/2024

[set-of-changes]: https://github.com/6zip-project/6zip/compare/v20.1.3...6zip-project:v21.1.4
