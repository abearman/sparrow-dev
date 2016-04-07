import time
from drone_utility import get_tango_location

while 1:
    print "[Tango location]:" + str(get_tango_location())
    time.sleep(1)
    
