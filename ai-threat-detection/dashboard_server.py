from flask import Flask, render_template_string
from flask_socketio import SocketIO, emit
import json
import time
import threading
from kafka import KafkaConsumer
import logging

# Suppress Kafka logging
logging.getLogger('kafka').setLevel(logging.WARNING)

app = Flask(__name__)
app.config['SECRET_KEY'] = 'threat-detection-dashboard'
socketio = SocketIO(app, cors_allowed_origins="*")

# Dashboard statistics
dashboard_stats = {
    'total_events': 0,
    'threats_detected': 0,
    'risk_scores': [],
    'attack_types': {
        'failed_login': 0,
        'privilege_escalation': 0,
        'large_file_download': 0,
        'after_hours_access': 0,
        'port_scan_detected': 0
    },
    'attackers': {}
}

class DashboardKafkaConsumer:
    def __init__(self):
        self.consumer = None
        self.running = False
        
    def start_consuming(self):
        """Start consuming from Kafka topics"""
        try:
            self.consumer = KafkaConsumer(
                'security-events',
                'threat-alerts',
                bootstrap_servers=['localhost:9092'],
                value_deserializer=lambda v: json.loads(v.decode('utf-8')),
                auto_offset_reset='latest'
            )
            
            self.running = True
            print("🔄 Connected to Kafka - streaming to dashboard...")
            
            for message in self.consumer:
                if not self.running:
                    break
                    
                # Process the event
                event_data = message.value
                self.process_event(event_data)
                
        except Exception as e:
            print(f"❌ Kafka connection error: {e}")
            print("🔄 Using simulated data instead...")
            self.simulate_data()
    
    def process_event(self, event):
        """Process a security event for the dashboard"""
        # Update statistics
        dashboard_stats['total_events'] += 1
        
        # Determine if it's a threat
        risk_score = event.get('risk_score', 0)
        if 'ml_analysis' in event:
            risk_score = event['ml_analysis'].get('risk_score', 0)
            is_threat = event['ml_analysis'].get('is_anomaly', False)
        else:
            is_threat = risk_score > 70
        
        if is_threat:
            dashboard_stats['threats_detected'] += 1
        
        # Update risk scores
        dashboard_stats['risk_scores'].append(risk_score)
        if len(dashboard_stats['risk_scores']) > 50:
            dashboard_stats['risk_scores'].pop(0)
        
        # Update attack types
        event_type = event.get('event_type', 'unknown')
        if event_type in dashboard_stats['attack_types']:
            dashboard_stats['attack_types'][event_type] += 1
        
        # Update attackers
        src_ip = event.get('src_ip', 'unknown')
        if src_ip != 'unknown':
            dashboard_stats['attackers'][src_ip] = dashboard_stats['attackers'].get(src_ip, 0) + 1
        
        # Create dashboard event
        dashboard_event = {
            'user_id': event.get('user_id', 'unknown'),
            'event_type': event_type,
            'src_ip': src_ip,
            'risk_score': risk_score,
            'is_threat': is_threat,
            'timestamp': event.get('timestamp', time.time()),
            'explanation': event.get('ml_analysis', {}).get('explanation', 'AI Analysis'),
            'stats': dashboard_stats.copy()
        }
        
        # Emit to all connected clients
        socketio.emit('new_threat', dashboard_event)
        
        print(f"📊 Dashboard: {event_type} | Risk: {risk_score:.1f} | User: {event.get('user_id', 'unknown')}")
    
    def simulate_data(self):
        """Simulate data if Kafka is not available"""
        import random
        
        threat_types = ['failed_login', 'privilege_escalation', 'large_file_download', 'after_hours_access']
        users = ['admin', 'root', 'alice', 'bob', 'guest']
        ips = ['203.0.113.45', '198.51.100.123', '192.0.2.99', '192.168.1.50']
        
        while self.running:
            event = {
                'user_id': random.choice(users),
                'event_type': random.choice(threat_types),
                'src_ip': random.choice(ips),
                'risk_score': random.uniform(20, 95),
                'timestamp': time.time()
            }
            
            self.process_event(event)
            time.sleep(3 + random.random() * 4)  # 3-7 second intervals
    
    def stop(self):
        """Stop the consumer"""
        self.running = False
        if self.consumer:
            self.consumer.close()

# Global consumer instance
kafka_consumer = DashboardKafkaConsumer()

@app.route('/')
def dashboard():
    """Serve the main dashboard"""
    # The HTML content is embedded here - in production, you'd use a template file
    with open('dashboard.html', 'r') as f:
        dashboard_html = f.read()
    return dashboard_html

@socketio.on('connect')
def handle_connect():
    """Handle client connection"""
    print('🔗 Dashboard client connected')
    # Send current stats to new client
    emit('dashboard_stats', dashboard_stats)

@socketio.on('disconnect')
def handle_disconnect():
    """Handle client disconnect"""
    print('📴 Dashboard client disconnected')

def start_kafka_consumer():
    """Start Kafka consumer in background thread"""
    kafka_consumer.start_consuming()

if __name__ == '__main__':
    print("🚀 Starting AI Threat Detection Dashboard Server")
    print("=" * 60)
    print("🌐 Dashboard URL: http://localhost:5000")
    print("📊 Real-time threat visualization")
    print("🔄 Connecting to Kafka...")
    print("-" * 60)
    
    # Start Kafka consumer in background
    consumer_thread = threading.Thread(target=start_kafka_consumer, daemon=True)
    consumer_thread.start()
    
    # Start Flask-SocketIO server
    try:
        socketio.run(app, host='0.0.0.0', port=5000, debug=False)
    except KeyboardInterrupt:
        print("\n🛑 Stopping dashboard server...")
        kafka_consumer.stop()
