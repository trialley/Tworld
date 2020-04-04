from flask_restful import reqparse, abort, Api, Resource
from flask import Flask
from flask import render_template
app = Flask(__name__, static_folder='../PCclient', static_url_path='')
@app.route('/')
def index():
    return app.send_static_file("index.html")


if __name__ == '__main__':

    # 张三负责qzone组件
    from qzone import add_qzone_routes
    add_qzone_routes(app)
    # qzone组件注册完成

    # 李四负责好友组件
    from friend import add_friend_routes
    add_friend_routes(app)
    # 好友组件注册完成

    app.run(port=80, debug=True)
