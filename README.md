# 7 Oct 2018

Updated with a new API key.

For macOS: You may update the API key with your own!

Step 1: Obtain your own key from IVLE [here](https://ivle.nus.edu.sg/LAPI/default.aspx).
Step 2: Copy paste the following line to terminal: perl -pi -e 's/oldAPIkey/yourAPIkey/g' /path/to/IVLEDownloader.app/Contents/MacOS/IVLEDownloader
where oldAPIkey might be either k0z3B5Ng9rhy3MKVAKsGG or dy1R6cs39qzjQamrIzX6U depending on which version you downloaded.

Credits to whipermr5

---

# Overview

This is simple Qt daemon to automatically download [National University of Singapore](http://www.nus.edu.sg/)'s
[Integrated Virtual Learning Environment](http://ivle.nus.edu.sg/) workbin files.

You can read more about it [here](http://yjyao.com/2012/08/nus-ivle-downloader.html).

---

# Developer Notes

In order to compile the source code, Qt SDK 5.5.1 is the last version you may use as newer versions do not include qtwebkit. Qt SDK may be downloaded [here](https://www.qt.io).

When compiling your own app, its recommended to change the APIKEY in the .pro file to your own!

Useful links [here](http://doc.qt.io/qt-5/osx-deployment.html) and [here](https://wiki.qt.io/Build_Standalone_Qt_Application_for_Windows

Troubleshooting qmake [here](https://stackoverflow.com/questions/22116608/cannot-run-qmake-in-mac-terminal)

---
