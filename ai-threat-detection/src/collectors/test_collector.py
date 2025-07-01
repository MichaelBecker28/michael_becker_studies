import time
import random
from typing import Dict, Any
from .base_collector import BaseLogCollector

class TestLogCollector(BaseLogCollector):
    """Test collector that generates sample security events"""
    
    def collect_logs(self):
        """Generate test security events"""
        event_types = [
            'user_login', 'user_logout', 'failed_login', 
            'file_access', 'network_connection', 'admin_action'
        ]
        
        users = ['alice', 'bob', 'charlie', 'admin', 'guest']
        ips = ['192.168.1.10', '192.168.1.20', '10.0.0.15', '172.16.0.5']
        
        print("🚀 Starting test log collection...")
        print("📊 Generating sample security events...")
        
        while self.is_running:
            # Generate random event
            event = {
                'timestamp': time.time(),
                'event_type': random.choice(event_types),
                'user_id': random.choice(users),
                'src_ip': random.choice(ips),
                'severity': random.choice(['low', 'medium', 'high']),
                'message': f"Test security event at {time.strftime('%H:%M:%S')}"
            }
            
            # Normalize and send
            normalized_event = self.normalize_log_entry(str(event))
            normalized_event.update(event)  # Add event fields
            
            self.send_to_kafka('security-events', normalized_event, key=event['user_id'])
            
            print(f"📝 Generated: {event['event_type']} from {event['user_id']} at {event['src_ip']}")
            
            # Wait before next event
            time.sleep(random.uniform(1, 5))  # Random delay 1-5 seconds
