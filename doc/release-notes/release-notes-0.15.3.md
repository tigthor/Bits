GameCredits Core version *0.15.3* is now available from:

  <https://github.com/gamecredits-project/GameCredits/releases>

This is a minor release fixing CVE-2018-17144 and another bug.

Please report bugs using the issue tracker at GitHub:

  <https://github.com/gamecredits-project/gamecredits/issues>

To receive security and update notifications, please subscribe to:

  <https://www.gamecredits.org>
  <https://twitter.com/gamecredits>
  <https://discordapp.com/invite/vDEYBTc/>

How to Upgrade
==============

If you are running an older version, shut it down. Wait until it has completely
shut down (which might take a few minutes for older versions), then run the 
installer (on Windows) or just copy over `/Applications/GameCredits-Qt` (on Mac)
or `gamecreditsd`/`gamecredits-qt` (on Linux).

The first time you run version 0.15.3, you may be asked to re-index your blockchain. This process will take anywhere from a few minutes to
several hours, depending on the speed of your machine.

There are no upgrades compared to 0.15.2 other than the bugfixes, however it is recommended to update the software to ensure local nodes are not vulnerable to CVE-2018-17144

Compatibility
==============

GameCredits Core is extensively tested on multiple operating systems using
the Linux kernel, macOS 10.8+, and Windows Vista and later. Windows XP is not supported.

GameCredits Core should also work on most other Unix-like systems but is not
frequently tested on them.

Notable changes
===============

Denial-of-Service vulnerability
-------------------------
A denial-of-service vulnerability (CVE-2018-17144) exploitable by miners has been discovered in Bitcoin Core versions 0.14.0 and up. The fix has been ported to GameCredits Core 0.15.2. It is recommended to upgrade any of the vulnerable versions to 0.15.3 as soon as possible.

Fix crash on listsinceblock RPC call
-------------------------
A bug fix has been implemented for listsinceblock RPC call. This resolves the bug where listsinceblock RPC call crashes when no block hash is provided.


0.15.3 Change log
====================

-  Fix DoS Vulnerability (CVE-2018-17144)
-  Fix listsinceblock RPC call
-  Update branding to correct domain (gamecredits.org)
-  Add in a Komodo notarization patch activating at 2177400


0.15.3 Known Issues
====================

-  Regtest network remains broken, but will be fixed in a future update.

Credits
=======

Thanks to everyone who directly contributed to this release:

- Samad Sajanlal
- MihailoGC
- CryptoDJ
- jl777 of Komodo