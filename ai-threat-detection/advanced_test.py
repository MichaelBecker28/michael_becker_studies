import time
import random
import signal
import sys
from src.collectors.test_collector import TestLogCollector

class AdvancedThreatTest(TestLogCollector):
    """Advanced threat simulator with realistic attack scenarios"""
    
    def collect_logs(self):
        print("🚨 Advanced AI Threat Detection Demo")
        print("=" * 50)
        print("Simulating realistic security scenarios...")
        print("🔴 Red = Threats  |  🟡 Yellow = Suspicious  |  🟢 Green = Normal")
        print("-" * 50)
        
        scenarios = [
            self.simulate_brute_force_attack,
            self.simulate_privilege_escalation, 
            self.simulate_data_exfiltration,
            self.simulate_insider_threat,
            self.simulate_normal_activity,
            self.simulate_normal_activity,  # More normal activity
            self.simulate_normal_activity
        ]
        
        while self.is_running:
            scenario = random.choice(scenarios)
            scenario()
            time.sleep(random.uniform(1, 4))  # Random delays
    
    def simulate_brute_force_attack(self):
        """Simulate a brute force password attack"""
        attacker_ip = random.choice(["203.0.113.45", "198.51.100.123", "192.0.2.99"])
        target_users = ["admin", "root", "administrator", "guest"]
        
        print("🔴 Simulating BRUTE FORCE ATTACK...")
        
        # Generate multiple failed login attempts
        for attempt in range(random.randint(5, 12)):
            event = {
                'timestamp': time.time(),
                'event_type': 'failed_login',
                'user_id': random.choice(target_users),
                'src_ip': attacker_ip,
                'severity': 'high',
                'message': f"Brute force attempt #{attempt+1} from {attacker_ip}",
                'login_method': 'ssh'
            }
            
            normalized_event = self.normalize_log_entry(str(event))
            normalized_event.update(event)
            self.send_to_kafka('security-events', normalized_event, key=event['user_id'])
            
            print(f"    💥 Failed login: {event['user_id']} from {attacker_ip}")
            time.sleep(0.3)  # Rapid attempts
    
    def simulate_privilege_escalation(self):
        """Simulate privilege escalation attempt"""
        event = {
            'timestamp': time.time(),
            'event_type': 'privilege_escalation', 
            'user_id': 'guest',
            'src_ip': '192.168.1.50',
            'severity': 'critical',
            'message': 'Guest user attempting to gain admin privileges',
            'target_privilege': 'administrator',
            'method': 'sudo_abuse'
        }
        
        normalized_event = self.normalize_log_entry(str(event))
        normalized_event.update(event)
        self.send_to_kafka('security-events', normalized_event, key=event['user_id'])
        
        print(f"🔴 PRIVILEGE ESCALATION: {event['user_id']} → {event['target_privilege']}")
    
    def simulate_data_exfiltration(self):
        """Simulate suspicious data download"""
        suspicious_user = random.choice(['alice', 'bob', 'charlie'])
        
        event = {
            'timestamp': time.time(),
            'event_type': 'large_file_download',
            'user_id': suspicious_user,
            'src_ip': '192.168.1.25',
            'severity': 'medium',
            'file_size': f"{random.uniform(1.5, 5.0):.1f}GB",
            'file_type': random.choice(['database_dump', 'customer_data', 'financial_records']),
            'message': 'Unusually large file download detected'
        }
        
        normalized_event = self.normalize_log_entry(str(event))
        normalized_event.update(event)
        self.send_to_kafka('security-events', normalized_event, key=event['user_id'])
        
        print(f"🟡 DATA EXFILTRATION: {suspicious_user} downloaded {event['file_size']} of {event['file_type']}")
    
    def simulate_insider_threat(self):
        """Simulate suspicious insider activity"""
        insider = random.choice(['alice', 'bob'])
        
        # Late night activity
        event = {
            'timestamp': time.time(),
            'event_type': 'after_hours_access',
            'user_id': insider,
            'src_ip': '192.168.1.15',
            'severity': 'medium',
            'time_of_day': 'night',
            'accessed_resources': random.choice(['hr_database', 'financial_system', 'customer_db']),
            'message': f'After-hours access to sensitive systems by {insider}'
        }
        
        normalized_event = self.normalize_log_entry(str(event))
        normalized_event.update(event)
        self.send_to_kafka('security-events', normalized_event, key=event['user_id'])
        
        print(f"🟡 INSIDER THREAT: {insider} accessing {event['accessed_resources']} after hours")
    
    def simulate_reconnaissance(self):
        """Simulate network reconnaissance/port scanning"""
        scanner_ip = "203.0.113.67"
        
        event = {
            'timestamp': time.time(),
            'event_type': 'port_scan_detected',
            'user_id': 'unknown',
            'src_ip': scanner_ip,
            'severity': 'high',
            'ports_scanned': random.randint(50, 200),
            'scan_type': 'tcp_syn_scan',
            'message': f'Port scanning activity from {scanner_ip}'
        }
        
        normalized_event = self.normalize_log_entry(str(event))
        normalized_event.update(event)
        self.send_to_kafka('security-events', normalized_event, key=scanner_ip)
        
        print(f"🔴 RECONNAISSANCE: Port scan from {scanner_ip} ({event['ports_scanned']} ports)")
    
    def simulate_normal_activity(self):
        """Simulate normal business activity"""
        normal_users = ['alice', 'bob', 'charlie', 'diana', 'eve']
        normal_events = [
            'user_login', 'file_access', 'email_sent', 'document_edit',
            'meeting_join', 'report_generated', 'user_logout'
        ]
        
        user = random.choice(normal_users)
        event_type = random.choice(normal_events)
        
        event = {
            'timestamp': time.time(),
            'event_type': event_type,
            'user_id': user,
            'src_ip': random.choice(['192.168.1.10', '192.168.1.20', '192.168.1.30', '192.168.1.40']),
            'severity': 'low',
            'message': f'Normal {event_type.replace("_", " ")} activity',
            'department': random.choice(['engineering', 'sales', 'hr', 'finance'])
        }
        
        normalized_event = self.normalize_log_entry(str(event))
        normalized_event.update(event)
        self.send_to_kafka('security-events', normalized_event, key=event['user_id'])
        
        print(f"🟢 Normal: {user} - {event_type.replace('_', ' ')}")

def signal_handler(sig, frame):
    print('\n🛑 Stopping advanced threat simulation...')
    collector.stop()
    sys.exit(0)

if __name__ == "__main__":
    print("🎯 Advanced AI Threat Detection Demo")
    print("=" * 50)
    print("This will generate realistic security threats for AI analysis")
    print("Press Ctrl+C to stop")
    print("-" * 50)
    
    # Create test collector
    config = {'source_name': 'advanced_threat_simulator'}
    collector = AdvancedThreatTest(config)
    
    # Setup signal handler for Ctrl+C
    signal.signal(signal.SIGINT, signal_handler)
    
    try:
        collector.start()
    except KeyboardInterrupt:
        collector.stop()
        print("\n✅ Advanced threat simulation completed!")
