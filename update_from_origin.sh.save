#!/bin/sh

git remote -v

#origin	git@github.com:KS4Nguyen/zmqpp_pty.git (fetch)
#origin	git@github.com:KS4Nguyen/zmqpp_pty.git (push)

# Set ZMQPP as upstream to fetch latest updates
git remote add upstream https://github.com/zeromq/zmqpp.git
git fetch upstream

git add .
git commit -m "Integration of ZMQPP_PTY features in the fork of the original ZMQPP (https://github.com/zeromq/zmqpp)"
# OPTIONAL git push origin main


# Get and integrate updates from ZMQPP
git fetch upstream
git checkout main               # oder der Branch, den du up-to-date halten willst
git merge upstream/main         # oder: git pull upstream main
# bei Bedarf Konflikte lösen und committen



# OPTIONAL Feature branch for ZMQPP_PTY

git checkout -b feature/zmqpp_pty
# Änderungen integrieren, committen
git push -u origin feature/zmqpp_pty

# In case upstrea/main is changing get the updates on main and rebase the
# feature branch with:

#	git fetch upstream
#	git checkout main
#	git merge upstream/main
#	git checkout feature/b-integration
#	git rebase main
#	git push --force-with-lease

