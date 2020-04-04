from flask import Flask, redirect, url_for, request


def add_qzone_routes(app):
    @app.route("/qzone", methods=['GET'])
    def getqzone():
        return 'test'

    @app.route("/qzone", methods=['POST'])
    def postqzone():
        return 'test'
