GameCredits Core version *0.15.2* is now available from:

  <https://www.gamecredits.org>

This is a major release introducing additional security features.

Please report bugs using the issue tracker at GitHub:

  <https://github.com/gamecredits-project/gamecredits/issues>

To receive security and update notifications, please subscribe to:

  <https://www.gamecredits.org>

How to Upgrade
==============

If you are running an older version, shut it down. Wait until it has completely
shut down (which might take a few minutes for older versions), then run the 
installer (on Windows) or just copy over `/Applications/GameCredits-Qt` (on Mac)
or `gamecreditsd`/`gamecredits-qt` (on Linux).

The first time you run version 0.15.2 or higher, you may be asked to re-index your blockchain. This process will take anywhere from a few minutes to
several hours, depending on the speed of your machine.

There are no upgrades compared to 0.15.1 other than the security updates, which require transaction indexing at this time. 

Compatibility
==============

GameCredits Core is extensively tested on multiple operating systems using
the Linux kernel, macOS 10.8+, and Windows Vista and later. Windows XP is not supported.

GameCredits Core should also work on most other Unix-like systems but is not
frequently tested on them.

Notable changes
===============

Komodo Notary Integration
-------------------------
We have integrated our software with Komodo's ecosystem to notarize our blockchain. This allows our blockchain to be "backed up" on the Komodo chain which we can restore from later (as long as we have at least one peer with the correct chain). The integration is expected to stop rollbacks of blocks due to consensus attacks that were observed in early May 2018.


Transaction indexing enabled by default
-------------------------
As part of the security features being introduced with the Komodo notary integration, we have turned transaction indexing on by default. This causes a reindex of the blocks on the user's device in case the user was not already indexing transactions. This process can take several hours to complete depending on the speed of the machine.


Message Signing String Bug fix
-------------------------

In 0.15.1, the message signing string was incorrectly written as "BITS Signed Message:". In 0.15.2, the string has been replaced with the correct version "BITS Signed Message:". Signed messages generated on 0.15.1 will fail verification on 0.15.2. However, older signed messages (from pre-0.15.1) will continue to work on 0.15.2.

Segwit and CSV Signaling Period Changed
-------------------------

Segwit and CSV signaling has moved from April 2019 to July 2018, lasting 1 year. An overlap period between April 2019 and July 2019 is present to ensure activation.

0.15.2 Change log
====================

-  Komodo notary integration
-  Transaction indexing is enabled by default
-  Message signing string has been updated to the correct one
-  Testnet and Regtest have been reintroduced with new genesis blocks
-  Transactions are now version 2
-  Getinfo now reports notarization status
-  Segwit and CSV signaling starts July 15, 2018 and lasts one year

Credits
=======

Thanks to everyone who directly contributed to this release:

- Samad Sajanlal
- jl777 of Komodo
- Vuksan Simunovic of GameCredits, Inc
- Nikola Divic of GameCredits, Inc