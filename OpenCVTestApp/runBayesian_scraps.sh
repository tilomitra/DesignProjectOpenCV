#!/bin/sh
PATH=$PATH:bin/`arch`
export PATH
run() {
    $@ || { echo Command failed: $@; echo; exit 1; }
}

# Creating directory for training data

run mkdir -p train/scraps/Bayesian

# Training the Bayesian classifier

run csuBayesianTrain -imDir data/csuScrapShots/normSep2002sfi imagelists/scrap_all.srt train/scraps/Bayesian/bayesian


