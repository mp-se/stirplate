import requests
import json

host = "192.168.1.157"       # Replace these with the IP of your device and ID of your device.
id = "7376ef"

def do_set_config( param, value ):

    url = 'http://' + host + '/api/config/set?'
    headers = { 'ContentType': 'text/plain' }

    resp = requests.get( url + "id=" + id + "&param=" + param + "&value=" + value, headers=headers, data="")

    if resp.status_code != 200 :
        print ( "Failed to set " + param )
    else :
        print ( "Success for " + param )


def do_config( ):

    url = 'http://' + host + '/config'
    headers = { 'ContentType': 'text/plain' }

    resp = requests.get( url, headers=headers, data="")

    if resp.status_code != 200 :
        print ( "Failed to get config" )
    else :
        print ( resp.json() )

cfg_empty           = ""
cfg_mdns            = "stirdev"
cfg_otaurl          = "" #"http://mywebserver/firmware/stirplate/"
cfg_httppush        = "" #"http://192.168.1.16:9090/api/v1/V63OKZ27q9mAOtp5CQQI/telemetry"
cfg_pushinterval    = "5"
cfg_blynkserver     = "" #"192.168.1.16"
cfg_blynkport       = "8080"
cfg_blynktoken      = "_iE5JRsn53t8AOaUzrBi3FW-ixBFFtM4"
cfg_tempformat      = "C"

do_set_config( "mdns", cfg_mdns )
do_set_config( "otaurl", cfg_otaurl )
do_set_config( "blynkserver", cfg_blynkserver )
do_set_config( "blynkserverport", cfg_blynkport )
do_set_config( "blynktoken", cfg_blynktoken )
do_set_config( "httppush", cfg_httppush )
do_set_config( "pushinterval", cfg_pushinterval )
do_set_config( "tempformat", cfg_tempformat )

do_config()
