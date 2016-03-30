import datetime
from flask import url_for
from server import db

class HTTPRequest(db.Document):
    _id = db.ObjectId()
    created_at = db.DateTimeField(default=datetime.datetime.now, required=True)
    path = db.StringField(max_length=255, required=True)
    request = db.StringField(max_length=255, required=True)

     def get_absolute_url(self):
        return url_for('http_request', kwargs={"request": self.request})

     def __unicode__(self):
        return self.request

     meta = {
        'allow_inheritance': True,
        'indexes': ['-created_at', 'request'],
        'ordering': ['-created_at']
    }



