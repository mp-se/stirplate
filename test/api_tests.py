import unittest
import requests
import json

host = "192.168.1.157"       # Replace these with the IP of your device and ID of your device.
id = "7376ef"

#
# Tests for checking that the device API works as planned 
#
def call_set_config( param, value, id2 ):
    url = 'http://' + host + '/api/config/set?'
    headers = { 'ContentType': 'text/plain' }
    return requests.get( url + "id=" + id2 + "&param=" + param + "&value=" + value, headers=headers, data="")

def call_get_config( param ):
    url = 'http://' + host + '/api/config/get?'
    headers = { 'ContentType': 'text/plain' }
    return requests.get( url + "param=" + param, headers=headers, data="")

def call_url( url, id2 ):
    url = 'http://' + host + url + '?id=' + id2
    headers = { 'ContentType': 'text/plain' }
    return requests.get( url, headers=headers, data="")

class ApiTests(unittest.TestCase):
  
    def test_setmdns(self):
        resp = call_set_config( "mdns", "myvalue", "wrong")
        self.assertEqual(resp.status_code, 400, "Should be 400")
        resp = call_set_config( "mdns", "myvalue", id)
        self.assertEqual(resp.status_code, 200, "Should be 200")
        resp = call_get_config( "mdns" )
        self.assertEqual( resp.json()["mdns"], "myvalue", "Wrong response." )

    def test_otaurl(self):
        resp = call_set_config( "otaurl", "myvalue", id)
        self.assertEqual(resp.status_code, 200, "Should be 200")
        resp = call_get_config( "otaurl" )
        self.assertEqual( resp.json()["otaurl"], "myvalue", "Wrong response." )

    def test_temp1(self):
        resp = call_set_config( "tempformat", "C", id)
        self.assertEqual(resp.status_code, 200, "Should be 200")
        resp = call_set_config( "TEMPFORMAT", "C", id)
        self.assertEqual(resp.status_code, 200, "Should be 200")
        resp = call_get_config( "tempformat" )
        self.assertEqual( resp.json()["tempformat"], "C", "Wrong response." )
        resp = call_set_config( "tempformat", "F", id)
        self.assertEqual(resp.status_code, 200, "Should be 200")
        resp = call_get_config( "tempformat" )
        self.assertEqual( resp.json()["tempformat"], "F", "Wrong response." )
        resp = call_set_config( "tempformat", "Q", id)
        self.assertEqual(resp.status_code, 400, "Should be 400")

    def test_httppush(self):
        resp = call_set_config( "httppush", "myvalue", id)
        self.assertEqual(resp.status_code, 200, "Should be 200")
        resp = call_get_config( "httppush" )
        self.assertEqual( resp.json()["httppush"], "myvalue", "Wrong response." )

    def test_blynkserver(self):
        resp = call_set_config( "blynkserver", "myvalue", id)
        self.assertEqual(resp.status_code, 200, "Should be 200")
        resp = call_get_config( "blynkserver" )
        self.assertEqual( resp.json()["blynkserver"], "myvalue", "Wrong response." )

    def test_blynkserverport(self):
        resp = call_set_config( "blynkserverport", "200", id)
        self.assertEqual(resp.status_code, 200, "Should be 200")
        resp = call_get_config( "blynkserverport" )
        self.assertEqual( resp.json()["blynkserverport"], 200, "Wrong response." )
        resp = call_set_config( "blynkserverport", "Q", id)
        self.assertEqual(resp.status_code, 400, "Should be 400")

    def test_blynktoken(self):
        resp = call_set_config( "blynktoken", "myvalue", id)
        self.assertEqual(resp.status_code, 200, "Should be 200")
        resp = call_get_config( "blynktoken" )
        self.assertEqual( resp.json()["blynktoken"], "myvalue", "Wrong response." )

    def test_reset(self):
        resp = call_url( "/reset", "wrong")
        self.assertEqual(resp.status_code, 400, "Should be 400")
        #resp = call_url( "/reset", id)
        #self.assertEqual(resp.status_code, 200, "Should be 200")

    def test_resetwifi(self):
        resp = call_url( "/clearwifi", "wrong")
        self.assertEqual(resp.status_code, 400, "Should be 400")

    def test_status(self):
        resp = call_url( "/status", id)
        print( resp.json() )
        self.assertEqual(resp.status_code, 200, "Should be 200")

    def test_config(self):
        resp = call_url( "/config", id)
        self.assertEqual(resp.status_code, 200, "Should be 200")
        self.assertEqual(resp.json()["id"], id, "Wrong response." )


if __name__ == '__main__':
    unittest.main()
