import json
import time
import threading
from kafka import KafkaConsumer, KafkaProducer
from .feature_engineer import SecurityFeatureEngineer
from .anomaly_detector import ThreatAnomalyDetector

class RealTimeThreatAnalyzer:
    """Real-time ML-powered threat analysis"""
    
    def __init__(self):
        self.feature_engineer = SecurityFeatureEngineer()
        self.anomaly_detector = ThreatAnomalyDetector()
        
        # Kafka setup
        self.consumer = KafkaConsumer(
            'security-events',
            bootstrap_servers=['localhost:9092'],
            value_deserializer=lambda v: json.loads(v.decode('utf-8')),
            auto_offset_reset='latest'  # Only process new events
        )
        
        self.producer = KafkaProducer(
            bootstrap_servers=['localhost:9092'],
            value_serializer=lambda v: json.dumps(v).encode('utf-8')
        )
        
        self.is_running = False
        self.events_processed = 0
        self.threats_detected = 0
        
    def start_analysis(self):
        """Start real-time threat analysis"""
        print("🚀 Starting Real-Time AI Threat Analysis")
        print("=" * 60)
        print("🧠 ML Models: Anomaly Detection")
        print("📊 Processing: security-events topic")
        print("🎯 Output: Real-time threat detection")
        print("-" * 60)
        
        self.is_running = True
        
        try:
            for message in self.consumer:
                if not self.is_running:
                    break
                
                # Process the security event
                event = message.value
                self._analyze_event(event)
                
        except KeyboardInterrupt:
            print("\n🛑 Stopping threat analysis...")
        finally:
            self.stop()
    
    def _analyze_event(self, event):
        """Analyze a single security event with ML"""
        try:
            self.events_processed += 1
            
            # Extract features
            features = self.feature_engineer.extract_features(event)
            
            # Detect anomalies
            anomaly_result = self.anomaly_detector.detect_anomaly(features)
            
            # Create analysis result
            analysis = {
                'timestamp': time.time(),
                'original_event': event,
                'features': features,
                'ml_analysis': anomaly_result,
                'events_processed': self.events_processed
            }
            
            # Display results
            self._display_analysis(event, anomaly_result)
            
            # If threat detected, send alert
            if anomaly_result['is_anomaly']:
                self.threats_detected += 1
                self._send_threat_alert(analysis)
            
        except Exception as e:
            print(f"❌ Error analyzing event: {e}")
    
    def _display_analysis(self, event, anomaly_result):
        """Display analysis results in real-time"""
        user_id = event.get('user_id', 'unknown')
        event_type = event.get('event_type', 'unknown')
        src_ip = event.get('src_ip', 'unknown')
        risk_score = anomaly_result['risk_score']
        
        # Color coding based on risk
        if anomaly_result['is_anomaly']:
            if risk_score >= 90:
                status = "🚨 CRITICAL THREAT"
                color = "\033[91m"  # Red
            elif risk_score >= 70:
                status = "⚠️  HIGH RISK"
                color = "\033[93m"  # Yellow
            else:
                status = "🔍 SUSPICIOUS"
                color = "\033[94m"  # Blue
        else:
            status = "✅ NORMAL"
            color = "\033[92m"  # Green
        
        reset_color = "\033[0m"
        
        print(f"{color}{status}{reset_color} | "
              f"User: {user_id:8} | "
              f"Event: {event_type:15} | "
              f"IP: {src_ip:15} | "
              f"Risk: {risk_score:5.1f} | "
              f"Events: {self.events_processed:4}")
        
        # Show explanation for threats
        if anomaly_result['is_anomaly']:
            print(f"    🔍 Reason: {anomaly_result['explanation']}")
            print(f"    📊 Stats: Threats={self.threats_detected}, Processed={self.events_processed}")
    
    def _send_threat_alert(self, analysis):
        """Send threat alert to alerts topic"""
        alert = {
            'alert_id': f"threat_{int(time.time())}_{self.threats_detected}",
            'timestamp': time.time(),
            'severity': self._calculate_severity(analysis['ml_analysis']['risk_score']),
            'threat_type': 'anomalous_behavior',
            'user_id': analysis['original_event'].get('user_id'),
            'src_ip': analysis['original_event'].get('src_ip'),
            'risk_score': analysis['ml_analysis']['risk_score'],
            'explanation': analysis['ml_analysis']['explanation'],
            'original_event': analysis['original_event']
        }
        
        # Send to threat-alerts topic
        self.producer.send('threat-alerts', value=alert)
    
    def _calculate_severity(self, risk_score):
        """Calculate threat severity based on risk score"""
        if risk_score >= 90:
            return 'CRITICAL'
        elif risk_score >= 70:
            return 'HIGH'
        elif risk_score >= 50:
            return 'MEDIUM'
        else:
            return 'LOW'
    
    def stop(self):
        """Stop the analyzer"""
        self.is_running = False
        self.consumer.close()
        self.producer.close()
        
        print(f"\n📊 Final Statistics:")
        print(f"   Events Processed: {self.events_processed}")
        print(f"   Threats Detected: {self.threats_detected}")
        print(f"   Detection Rate: {(self.threats_detected/max(1,self.events_processed))*100:.1f}%")
        print("✅ Threat analysis stopped!")

if __name__ == "__main__":
    analyzer = RealTimeThreatAnalyzer()
    analyzer.start_analysis()
