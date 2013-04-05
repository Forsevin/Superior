Superior
========

Superior is a file downloader with a repository like system inspired by package managers like Debians Apt.

NOTE: Most of these features are still in development.

Superior consists of two files (mainly)
* Sources
    - Superior pulls indexs from these URL and put it's content in the local index
* Index
    - All the files are defined in here, each line consists of "{name} {file} {version}"

Commands
========
* $ superior pull
    - Update index from sources
* $ superior get {file}
    - Locate a file in 'index'
* $ superior lf
    - List Files available in 'index'
* $ superior ls
    - List Sources available in 'sources'
* $ superior add-repo {url}
    - Add a source to 'sources'
* $ superior download {url}
    - Simply download a file from url
