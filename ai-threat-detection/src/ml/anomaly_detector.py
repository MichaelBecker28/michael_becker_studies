import numpy as np
from sklearn.ensemble import IsolationForest
from sklearn.preprocessing import StandardScaler
import joblib
import time

class ThreatAnomalyDetector:
    """ML model to detect anomalous security behavior"""
    
    def __init__(self):
        self.model = IsolationForest(
            contamination=0.15,  # Expect 15% of events to be anomalous
            random_state=42,
            n_jobs=-1
        )
        self.scaler = StandardScaler()
        self.is_trained = False
        self.feature_names = []
    
    def train_on_normal_data(self, normal_features_list):
        """Train the model on normal behavior data"""
        print("🧠 Training anomaly detection model...")
        
        if not normal_features_list:
            print("❌ No training data provided")
            return
        
        # Convert to numpy array
        feature_matrix = []
        self.feature_names = list(normal_features_list[0].keys())
        
        for features in normal_features_list:
            feature_vector = [features.get(name, 0.0) for name in self.feature_names]
            feature_matrix.append(feature_vector)
        
        feature_matrix = np.array(feature_matrix)
        
        # Scale features
        scaled_features = self.scaler.fit_transform(feature_matrix)
        
        # Train model
        self.model.fit(scaled_features)
        self.is_trained = True
        
        print(f"✅ Model trained on {len(normal_features_list)} normal behavior examples")
    
    def detect_anomaly(self, features: dict) -> dict:
        """Detect if features represent anomalous behavior"""
        if not self.is_trained:
            # Generate some synthetic normal data for quick training
            self._quick_train()
        
        # Convert features to vector
        feature_vector = [features.get(name, 0.0) for name in self.feature_names]
        feature_vector = np.array(feature_vector).reshape(1, -1)
        
        # Scale features
        scaled_features = self.scaler.transform(feature_vector)
        
        # Get prediction (-1 = anomaly, 1 = normal)
        prediction = self.model.predict(scaled_features)[0]
        
        # Get anomaly score (lower = more anomalous)
        anomaly_score = self.model.score_samples(scaled_features)[0]
        
        # Convert to risk score (0-100, higher = more risky)
        risk_score = max(0, min(100, (0.5 - anomaly_score) * 100))
        
        is_anomaly = prediction == -1 or risk_score > 70
        
        result = {
            'is_anomaly': is_anomaly,
            'risk_score': float(risk_score),
            'anomaly_score': float(anomaly_score),
            'confidence': abs(anomaly_score) * 100,
            'explanation': self._explain_anomaly(features, is_anomaly, risk_score)
        }
        
        return result
    
    def _quick_train(self):
        """Quick training on synthetic normal data if no training data available"""
        print("🔄 Quick-training on synthetic normal behavior...")
        
        # Generate synthetic normal behavior features
        normal_data = []
        for _ in range(1000):
            features = {
                'hour_of_day': np.random.uniform(0.25, 0.75),  # Business hours mostly
                'is_weekend': 0.0,
                'is_night_time': 0.0,
                'is_business_hours': 1.0,
                'user_events_last_hour': np.random.uniform(1, 10),
                'user_unique_ips_today': np.random.uniform(1, 3),
                'user_failed_login_ratio': np.random.uniform(0, 0.1),
                'ip_events_last_hour': np.random.uniform(1, 5),
                'ip_unique_users': np.random.uniform(1, 2),
                'ip_is_external': 0.0,
                'is_login_event': np.random.choice([0.0, 1.0]),
                'is_failed_event': 0.0,
                'is_admin_event': np.random.choice([0.0, 1.0]) * 0.1,  # Rare
                'new_ip_for_user': 0.0,
                'high_frequency_user': 0.0,
                'suspicious_timing': 0.0
            }
            normal_data.append(features)
        
        self.train_on_normal_data(normal_data)
    
    def _explain_anomaly(self, features: dict, is_anomaly: bool, risk_score: float) -> str:
        """Explain why something was flagged as anomalous"""
        if not is_anomaly:
            return "Normal behavior pattern"
        
        explanations = []
        
        if features.get('is_night_time', 0) and features.get('user_events_last_hour', 0) > 10:
            explanations.append("High activity during night hours")
        
        if features.get('user_failed_login_ratio', 0) > 0.3:
            explanations.append(f"High failed login ratio ({features.get('user_failed_login_ratio', 0):.1%})")
        
        if features.get('new_ip_for_user', 0):
            explanations.append("Login from new IP address")
        
        if features.get('high_frequency_user', 0):
            explanations.append("Unusually high user activity")
        
        if features.get('ip_is_external', 0) and features.get('is_admin_event', 0):
            explanations.append("Admin action from external IP")
        
        if features.get('suspicious_timing', 0):
            explanations.append("Suspicious timing pattern")
        
        explanation = "; ".join(explanations) if explanations else f"Statistical anomaly (risk score: {risk_score:.0f})"
        return explanation
