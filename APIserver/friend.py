from flask import Flask, redirect, url_for, request


def add_friend_routes(app):
    @app.route("/friend", methods=['GET'])
    def getfriend():
        return 'test'

    @app.route("/friend", methods=['POST'])
    def postfriend():
        return 'test'
