terminus
========

A Drush-based CLI interface into the Pantheon core API via a pseudoproxy.

This is being developed initially to support some enterprise and partner edge cases (Vodaware, Kalabox, proviso), but we would like to implement it for public release on drupal.org.

See the "terminus" section in `drush help` for a list of commands.

Quick Demo
==========

    git clone https://github.com/pantheon-systems/terminus.git $HOME/.drush/terminus
    drush pantheon-auth
    drush pantheon-sites
    drush pantheon-aliases

You'll find many more fun commands in "drush help".

TODO
====

- Site creation
- Site import
- Workflow operations
- Team management
- In-progress job status
- Websockets support for real-time status info?
- A "pantheon shell" option to keep people in context for interactive use
