import nmap
import json
import subprocess
import platform

class HomeNetworkScanner:
    def __init__(self):
        self.nm = nmap.PortScanner()
        self.devices = []
        
    def discover_network(self):
        """Discover all devices on home network"""
        print("🏠 Discovering Home Network Devices...")
        
        # Get your network range (usually 192.168.1.0/24 or 192.168.0.0/24)
        network_range = self.get_network_range()
        print(f"📡 Scanning network: {network_range}")
        
        # Scan for active devices
        self.nm.scan(hosts=network_range, arguments='-sn')  # Ping scan
        
        for host in self.nm.all_hosts():
            if self.nm[host].state() == 'up':
                device_info = self.get_device_info(host)
                self.devices.append(device_info)
                print(f"📱 Found: {device_info['name']} ({device_info['ip']})")
        
        return self.devices
    
    def get_network_range(self):
        """Auto-detect your home network range"""
        if platform.system() == "Windows":
            result = subprocess.run(['ipconfig'], capture_output=True, text=True)
            # Parse Windows ipconfig output
            lines = result.stdout.split('\n')
            for line in lines:
                if 'IPv4 Address' in line and '192.168' in line:
                    ip = line.split(':')[1].strip()
                    return f"{'.'.join(ip.split('.')[:-1])}.0/24"
        else:
            result = subprocess.run(['ip', 'route'], capture_output=True, text=True)
            # Parse Linux ip route output
            lines = result.stdout.split('\n')
            for line in lines:
                if 'src 192.168' in line:
                    parts = line.split()
                    for part in parts:
                        if '192.168' in part and '/' in part:
                            return part
        
        # Default fallback
        return "192.168.1.0/24"
    
    def get_device_info(self, ip):
        """Get detailed device information"""
        device = {
            'ip': ip,
            'hostname': 'Unknown',
            'mac': 'Unknown',
            'vendor': 'Unknown',
            'device_type': 'Unknown',
            'os': 'Unknown'
        }
        
        # Try to get hostname
        try:
            import socket
            device['hostname'] = socket.gethostbyaddr(ip)[0]
        except:
            pass
        
        # Get MAC address and vendor
        if ip in self.nm:
            if 'addresses' in self.nm[ip]:
                if 'mac' in self.nm[ip]['addresses']:
                    device['mac'] = self.nm[ip]['addresses']['mac']
                    device['vendor'] = self.get_vendor_from_mac(device['mac'])
        
        # Classify device type
        device['device_type'] = self.classify_device(device)
        device['name'] = self.generate_friendly_name(device)
        
        return device
    
    def classify_device(self, device):
        """Classify device type based on characteristics"""
        hostname = device['hostname'].lower()
        vendor = device['vendor'].lower()
        
        if any(x in hostname for x in ['router', 'gateway', 'modem']):
            return 'Router/Gateway'
        elif any(x in vendor for x in ['apple', 'iphone', 'ipad']):
            return 'Apple Device'
        elif any(x in vendor for x in ['samsung', 'android']):
            return 'Android Device'
        elif any(x in hostname for x in ['desktop', 'pc', 'laptop']):
            return 'Computer'
        elif any(x in hostname for x in ['smart', 'iot', 'alexa', 'google']):
            return 'IoT Device'
        else:
            return 'Unknown Device'
    
    def generate_friendly_name(self, device):
        """Generate user-friendly device name"""
        if device['hostname'] != 'Unknown' and device['hostname'] != device['ip']:
            return f"{device['hostname']} ({device['device_type']})"
        else:
            return f"{device['device_type']} - {device['ip']}"
    
    def save_network_map(self, filename='home_network_map.json'):
        """Save network map for reference"""
        with open(filename, 'w') as f:
            json.dump(self.devices, f, indent=2)
        print(f"💾 Network map saved to {filename}")

if __name__ == "__main__":
    scanner = HomeNetworkScanner()
    devices = scanner.discover_network()
    scanner.save_network_map()
    
    print(f"\n🏠 Network Discovery Complete!")
    print(f"📊 Found {len(devices)} devices on your network")
