# Dashboard is opened for submissions for a 24 hour period starting at
# the specified NIGHLY_START_TIME. Time is specified in 24 hour format.
SET (NIGHTLY_START_TIME "21:00:00 EDT")

SET (DROP_METHOD "ftp")
SET (DROP_SITE "bsl-1.ece.vt.edu")
SET (DROP_LOCATION "/incoming")
SET (DROP_SITE_USER "anonymous")
SET (DROP_SITE_PASSWORD "public")

SET (TRIGGER_SITE 
  "http://${DROP_SITE}/cgi-bin/Submit-CTC-TestingResults.cgi")

# Project Home Page
SET (PROJECT_URL "http://www.bsl.ece.vt.edu")

# Dart server configuration 
#SET (ROLLUP_URL "http://${DROP_SITE}/cgi-bin/insight-rollup-dashboard.sh")
#SET (CVS_WEB_URL "http://${DROP_SITE}/cgi-bin/viewcvs.cgi/")
#SET (CVS_WEB_CVSROOT "Insight")

#OPTION(BUILD_DOXYGEN "Build source documentation using doxygen" "Off")
#SET (DOXYGEN_CONFIG "${PROJECT_BINARY_DIR}/doxygen.config" )
#SET (USE_DOXYGEN "On")
#SET (DOXYGEN_URL "http://${DROP_SITE}/Insight/Doxygen/html/" )

#SET (USE_GNATS "On")
#ET (GNATS_WEB_URL "http://${DROP_SITE}/Bug/query.php?projects=6&status%5B%5D=1&status%5B%5D=2&status%5B%5D=3&status%5B%5D=4&status%5B%5D=6&op=doquery")


#MARK_AS_ADVANCED(BUILD_DOXYGEN)
