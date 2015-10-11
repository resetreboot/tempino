#!/usr/bin/env python

import serial
import sensors
import os
import re
import subprocess
import time
import signal

running = True

# Be careful if you use Python 3, it'll send Unicode and
# the Arduino won't know what to do with it. Add a b before
# the strings or use the conversion functions before sending

def signal_handler(signal, frame):
    global running
    print("Exiting...")
    running = False


def available_cpu_count():
    """ Number of available virtual or physical CPUs on this system, i.e.
    user/real as output by time(1) when called with an optimally scaling
    userspace-only program"""

    # cpuset
    # cpuset may restrict the number of *available* processors
    try:
        m = re.search(r'(?m)^Cpus_allowed:\s*(.*)$',
                      open('/proc/self/status').read())
        if m:
            res = bin(int(m.group(1).replace(',', ''), 16)).count('1')
            if res > 0:
                return res
    except IOError:
        pass

    # Python 2.6+
    try:
        import multiprocessing
        return multiprocessing.cpu_count()
    except (ImportError, NotImplementedError):
        pass

    # http://code.google.com/p/psutil/
    try:
        import psutil
        return psutil.cpu_count()   # psutil.NUM_CPUS on old versions
    except (ImportError, AttributeError):
        pass

    # POSIX
    try:
        res = int(os.sysconf('SC_NPROCESSORS_ONLN'))

        if res > 0:
            return res
    except (AttributeError, ValueError):
        pass

    # Windows
    try:
        res = int(os.environ['NUMBER_OF_PROCESSORS'])

        if res > 0:
            return res
    except (KeyError, ValueError):
        pass

    # jython
    try:
        from java.lang import Runtime
        runtime = Runtime.getRuntime()
        res = runtime.availableProcessors()
        if res > 0:
            return res
    except ImportError:
        pass

    # BSD
    try:
        sysctl = subprocess.Popen(['sysctl', '-n', 'hw.ncpu'],
                                  stdout=subprocess.PIPE)
        scStdout = sysctl.communicate()[0]
        res = int(scStdout)

        if res > 0:
            return res
    except (OSError, ValueError):
        pass

    # Linux
    try:
        res = open('/proc/cpuinfo').read().count('processor\t:')

        if res > 0:
            return res
    except IOError:
        pass

    # Solaris
    try:
        pseudoDevices = os.listdir('/devices/pseudo/')
        res = 0
        for pd in pseudoDevices:
            if re.match(r'^cpuid@[0-9]+$', pd):
                res += 1

        if res > 0:
            return res
    except OSError:
        pass

    # Other UNIXes (heuristic)
    try:
        try:
            dmesg = open('/var/run/dmesg.boot').read()
        except IOError:
            dmesgProcess = subprocess.Popen(['dmesg'], stdout=subprocess.PIPE)
            dmesg = dmesgProcess.communicate()[0]

        res = 0
        while '\ncpu' + str(res) + ':' in dmesg:
            res += 1

        if res > 0:
            return res
    except OSError:
        pass

    raise Exception('Can not determine number of CPUs on this system')


class Data(object):
    """
    Data encapsulation
    """
    def __init__(self, label, value):
        self.label = label
        if (label is not None):
            self.value = int(value)

        else:
            self.value = value

    def __str__(self):
        return self.label + " {}".format(self.value)


class Sensors(object):
    """
    Parses the output of the sensors command
    """
    def __init__(self):
        print("Getting sensors ready")
        sensors.init()
        self.data = {}
        self.cpus = available_cpu_count()

        self.update()

    def update(self):
        print("Reading sensors")
        cpu_counter = 0
        fan_count = 0

        for chip in sensors.iter_detected_chips():
            for feature in chip:
                if feature.get_value() > 0 and feature.label != 'CPUTIN':
                    if 'cpu' in feature.label.lower() or 'core' in feature.label.lower() and cpu_counter < self.cpus:
                        self.data['CPU{}'.format(cpu_counter)] = Data(feature.label, feature.get_value())
                        cpu_counter += 1

                    elif 'fan' in feature.label.lower():
                        self.data['FAN{}'.format(fan_count)] = Data(feature.label, feature.get_value())
                        fan_count += 1

                    elif 'sys' in feature.label.lower():
                        self.data['SYSTEM'] = Data(feature.label, feature.get_value())


class Arduino(object):
    """
    With this class we communicate with the Arduino
    """
    def __init__(self):
        # TODO: Should be configurable or read from command line
        print("Connecting to Arduino")
        self.serial = serial.Serial('/dev/ttyACM0', 9600)
        time.sleep(2)

    def close(self):
        self.serial.close()

    def send(self, sensors_data):
        print("Sending data")
        sensors_data.update()
        data = sensors_data.data

        stream = []

        keys = data.keys()
        keys.sort()

        for key in keys:
            if 'CPU' in key:
                stream.append("CPU={}".format(data[key]))

            if 'FAN' in key:
                stream.append("FAN={}".format(data[key]))

            if 'SYS' in key:
                stream.append("SYS={}".format(data[key]))

        print("Sending {} datagrams".format(len(stream)))

        res = ";".join(stream)

        print(res)

        self.serial.write(res + "\n")


if __name__ == "__main__":
    signal.signal(signal.SIGINT, signal_handler)
    arduino = Arduino()
    sensor_data = Sensors()

    while running:
        arduino.send(sensor_data)
        # This should be configurable too
        time.sleep(3)

    arduino.close()




