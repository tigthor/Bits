GameCredits Core version *0.15.4* is now available from:

  <https://github.com/gamecredits-project/GameCredits/releases>

This is a release that covers the seasonal change of Komodo notaries among other fixes. It is a MANDATORY UPGRADE.

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

The first time you run version 0.15.4, you may be asked to re-index your blockchain. This process will take anywhere from a few minutes to
several hours, depending on the speed of your machine.

This is a MANDATORY UPGRADE in order to remain on the correct chain of GameCredits (notarized by Komodo). Services must upgrade before block #2433888 to be on the correct chain at activation.

Compatibility
==============

GameCredits Core is extensively tested on multiple operating systems using
the Linux kernel, macOS 10.8+, and Windows Vista and later. Windows XP is not supported.

GameCredits Core should also work on most other Unix-like systems but is not
frequently tested on them.

Notable changes
===============

Komodo Seasonal Changes
-------------------------
Komodo's notaries are elected once a year and have a period of 1 year to service the network. New notaries were elected that go into effect on July 15th, 2019. This release ensures that GameCredits remains notarized throughout the regime change period and until the next notary elections.

Komodo dPoW Confirmations
-------------------------
We have enabled dPoW confirmations by default. When a transaction is accepted into a block on the network, it will receive 1 confirmation. The GameCredits daemon will continue to return this single confirmation until a notarization takes place. Once a notarization happens, the true confirmations will be returned. This feature allows services to accept GameCredits in a very secure manner - by ensuring that Komodo has notarized a transaction before crediting balances. As we mentioned in a previous release, Komodo's notarization system prevents 51% attacks on GameCredits.

Working Testnet
-------------------------
Testnet is fixed and working in this release. We request anyone developing on GameCredits to please run a testnet node and mine against it. You can use Raspberry Pi 3B's or higher to mine and receive testnet coins.

Updated DNS Seeders
-------------------------
We have shifted the DNS seeder into the <https://gamecredits.network> umbrella of services. Similarly, a testnet seeder has been launched and put under the same domain. Functionality is unaffected by this change.

0.15.4 Change log
====================

-  update seeds
-  add new pubkey activation code to GAME for KMD hardfork.
-  add dPoW conf functionality into GAME
-  add season 3 notaries


0.15.4 Known Issues
====================

-  Regtest network remains broken, but will be fixed in a future update.

Credits
=======

Thanks to everyone who directly contributed to this release:

- Samad Sajanlal
- blackjok3rtt
- dukeleto