#!/bin/sh

UPSTREAM="https://github.com/zeromq/zmqpp.git" # the ZMQPP
ORIGIN="git@github.com:KS4Nguyen/zmqpp_pty.git" # the fork


##############################################################
# Set ZMQPP as upstream and fetch latest updates
##############################################################

git remote -v | grep $UPSTREAM
if [ $? -ne 0 ]; then
   git remote add upstream $UPSTREAM
fi

# ZMQPP_PTY origin	git@github.com:KS4Nguyen/zmqpp_pty.git (fetch/push)
# ZMQPP     upstream	https://github.com/zeromq/zmqpp.git (fetch)

git fetch upstream
#git checkout <feature> # if you want to update from a feature-branch


##############################################################
# Execute the update
##############################################################

# First save local changes by committing them
git add .
git commit -m "Updating the latest ZMQPP from $UPSTREAM"

# Merge latest changes from ZMQPP into local work
#git merge upstream/master

git push origin

# Feature branch for ZMQPP_PTY
#git checkout -b feature/zmqpp_pty
#git push -u origin feature/zmqpp_pty


##############################################################
# Rebase in case upstrea/master has changed
##############################################################

#git fetch upstream
#git checkout master
#git merge upstream/master
#git checkout feature/b-integration
#git rebase master
#git push --force-with-lease

