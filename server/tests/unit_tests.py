import os
import sys
import tempfile
import unittest

sys.path.insert(0, '../')
import server

class ServerUnitTestCase(unittest.TestCase):

    def setUp(self):
        self.app = server.app.test_client()

if __name__ == '__main__':
    unittest.main()
