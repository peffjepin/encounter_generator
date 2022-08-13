#!/usr/bin/env python3

import flask
import sys
import os

# make sure that we are importing an installed version of the package
# and not importing the package directly from the source in this directory
sys.path.remove(os.path.abspath(os.path.dirname(sys.argv[0])))
# fmt: off
import encounters.flaskapp.app as bp
# fmt: on

app = flask.Flask(__name__)
app.register_blueprint(
    bp.encounters_blueprint, url_prefix="/encounter_generator")

if __name__ == "__main__":
    app.run(debug=True)
