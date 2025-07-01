import json
import logging
import time
from abc import ABC, abstractmethod
from typing import Dict, Any, Optional
from kafka import KafkaProducer

class BaseLogCollector(ABC):
    """Base class for all log collectors"""
    
    def __init__(self, config: Dict[str, Any]):
        self.config = config
        self.kafka_producer = KafkaProducer(
            bootstrap_servers=['localhost:9092'],
            value_serializer=lambda v: json.dumps(v).encode('utf-8'),
            key_serializer=lambda k: k.encode('utf-8') if k else None
        )
        self.logger = logging.getLogger(self.__class__.__name__)
        self.is_running = False
    
    @abstractmethod
    def collect_logs(self):
        """Implement log collection logic"""
        pass
    
    def normalize_log_entry(self, raw_log: str) -> Dict[str, Any]:
        """Normalize log entry to common format"""
        return {
            'timestamp': time.time(),
            'source': self.config.get('source_name', 'unknown'),
            'raw_message': raw_log,
            'event_type': 'generic_log',
            'processing_time': time.time()
        }
    
    def send_to_kafka(self, topic: str, log_entry: Dict[str, Any], key: Optional[str] = None):
        """Send log entry to Kafka topic"""
        try:
            self.kafka_producer.send(topic, value=log_entry, key=key)
            self.logger.info(f"Sent log entry to topic {topic}")
        except Exception as e:
            self.logger.error(f"Failed to send log to Kafka: {e}")
    
    def start(self):
        """Start the log collector"""
        self.is_running = True
        self.logger.info(f"Starting {self.__class__.__name__}")
        self.collect_logs()
    
    def stop(self):
        """Stop the log collector"""
        self.is_running = False
        self.kafka_producer.close()
        self.logger.info(f"Stopped {self.__class__.__name__}")
