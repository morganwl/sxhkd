"""Test that hotkeys can be grouped in configuration and selectively
disabled via signal."""

import unittest
import select
import subprocess
import os
import time

def send_key(key):
    """Sends a key to the X display."""
    subprocess.run(['xdotool', 'key', key])
    time.sleep(.01)

def send_toggle_group(pid, group):
    """Sends a toggle group command to sxhkd."""
    subprocess.run(['./sxhkd', '-u', str(pid), 'toggle', group])

def readline_if_available(stream, timeout=0):
    """Returns the next line from a stream, or an empty string if no
    output is available before timing out."""
    if select.select([stream], [], [], timeout)[0]:
        return stream.readline().strip()
    return ''

class TestHotkeyGroups(unittest.TestCase):
    """Tests hotkey group functionality."""

    def setUp(self):
        self.display = ':99'
        os.environ['DISPLAY'] = self.display
        self.server = subprocess.Popen(['Xvfb', self.display])
        time.sleep(.05)
        self.sxhkd = subprocess.Popen(['./sxhkd', '-c', 'test/functional/hk_groups_rc'],
                stdout=subprocess.PIPE, stderr=subprocess.PIPE, encoding='utf8')
        time.sleep(.05)

    def tearDown(self):
        self.sxhkd.stdout.close()
        self.sxhkd.stderr.close()
        self.sxhkd.terminate()
        self.sxhkd.communicate()
        self.server.terminate()
        self.server.communicate()

    def test_groups_at_start(self):
        """Verifies that all hotkey groups are enabled at startup."""
        stdout = self.sxhkd.stdout
        send_key('a')
        response = readline_if_available(stdout)
        self.assertEqual('Caught a', response)
        send_key('b')
        response = readline_if_available(stdout)
        self.assertEqual('Caught b', response)
        send_key('c')
        response = readline_if_available(stdout)
        self.assertEqual('Caught c', response)
        send_key('d')
        response = readline_if_available(stdout)
        self.assertEqual('Caught d', response)

    def test_groups_disable(self):
        """Verifies that hotkeys from a group, and only that group, are
        disabled when a signal is passed."""
        pid = self.sxhkd.pid
        stdout = self.sxhkd.stdout
        send_toggle_group(pid, 'A')
        send_key('a')
        response = readline_if_available(stdout)
        self.assertEqual('', response)
        send_key('b')
        response = readline_if_available(stdout)
        self.assertEqual('', response)
        
if __name__ == '__main__':
    unittest.main()
