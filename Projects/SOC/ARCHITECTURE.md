# 🏛️ System Architecture

Comprehensive architecture documentation for the AI-Powered SOC Dashboard.

---

## 📊 High-Level Architecture

```
┌──────────────────────────────────────────────────────────────────┐
│                         USER INTERFACE                            │
│                     React Dashboard (Port 3000)                   │
│  ┌─────────────┐  ┌──────────────┐  ┌─────────────────────────┐ │
│  │   Alerts    │  │ Investigation│  │   Threat Intelligence   │ │
│  │  Dashboard  │  │   Workflow   │  │       (RAG Query)       │ │
│  └─────────────┘  └──────────────┘  └─────────────────────────┘ │
└──────────────────────────────┬───────────────────────────────────┘
                               │ REST API / WebSocket
┌──────────────────────────────▼───────────────────────────────────┐
│                      APPLICATION LAYER                            │
│                    FastAPI Backend (Port 8000)                    │
│  ┌─────────────┐  ┌──────────────┐  ┌─────────────────────────┐ │
│  │   API       │  │   LangGraph  │  │    Event Processor      │ │
│  │  Gateway    │  │   Workflows  │  │   (Log Ingestion)       │ │
│  └─────────────┘  └──────────────┘  └─────────────────────────┘ │
└───────────┬──────────────────┬───────────────────┬───────────────┘
            │                  │                   │
┌───────────▼────────┐ ┌──────▼──────────┐ ┌──────▼──────────────┐
│   AI/ML LAYER      │ │  VECTOR STORE   │ │    DATA SOURCES     │
│   Ollama (Llama 3) │ │    ChromaDB     │ │   Wazuh SIEM API    │
│  (Port 11434)      │ │  (Embeddings)   │ │   (Port 55000)      │
└────────────────────┘ └─────────────────┘ └─────────────────────┘
```

---

## 🔧 Component Breakdown

### 1. Frontend Layer (React)

**Technology Stack:**
- React 18 with TypeScript
- TailwindCSS for styling
- Recharts & D3.js for visualization
- Axios for HTTP requests
- Socket.IO for WebSocket connections

**Key Components:**

```typescript
frontend/
├── src/
│   ├── components/
│   │   ├── Dashboard/
│   │   │   ├── AlertsWidget.tsx        # Real-time alerts display
│   │   │   ├── MetricsCard.tsx         # Security metrics (failed logins, etc.)
│   │   │   └── ThreatMap.tsx           # Geographic threat visualization
│   │   ├── Investigation/
│   │   │   ├── ChatInterface.tsx       # AI chat for queries
│   │   │   ├── Timeline.tsx            # Investigation timeline
│   │   │   └── IOCExtractor.tsx        # Display extracted IOCs
│   │   └── Shared/
│   │       ├── Navbar.tsx
│   │       └── Loader.tsx
│   ├── services/
│   │   ├── api.ts                      # API client (Axios)
│   │   └── websocket.ts                # WebSocket manager
│   ├── hooks/
│   │   ├── useWebSocket.ts             # Custom WebSocket hook
│   │   └── useAlerts.ts                # Alert data fetching
│   ├── types/
│   │   └── index.ts                    # TypeScript interfaces
│   └── App.tsx
└── package.json
```

**Data Flow:**
1. User enters query in `ChatInterface.tsx`
2. Query sent to `/api/investigate` endpoint
3. Response streamed back via WebSocket
4. Results displayed in real-time

---

### 2. Backend Layer (FastAPI)

**Technology Stack:**
- Python 3.11+
- FastAPI for REST API
- LangGraph for AI workflows
- ChromaDB for vector storage
- Ollama Python client

**Directory Structure:**

```python
backend/
├── app/
│   ├── main.py                    # FastAPI app initialization
│   ├── api/
│   │   ├── routes/
│   │   │   ├── alerts.py          # GET /alerts, GET /alerts/{id}
│   │   │   ├── investigate.py     # POST /investigate
│   │   │   ├── events.py          # POST /events/ingest
│   │   │   └── websocket.py       # WebSocket endpoint
│   │   └── dependencies.py        # Dependency injection
│   ├── core/
│   │   ├── config.py              # Environment configuration
│   │   ├── security.py            # Authentication (future)
│   │   └── logging.py             # Structured logging
│   ├── services/
│   │   ├── wazuh_client.py        # Wazuh API integration
│   │   ├── llm_service.py         # Ollama/Llama 3 wrapper
│   │   ├── vector_store.py        # ChromaDB operations
│   │   └── event_processor.py     # Log parsing & ingestion
│   ├── workflows/
│   │   ├── investigation.py       # LangGraph investigation workflow
│   │   ├── triage.py              # Alert triage workflow
│   │   └── ioc_extraction.py      # IOC extraction workflow
│   ├── models/
│   │   ├── event.py               # Pydantic models
│   │   ├── alert.py
│   │   └── investigation.py
│   └── utils/
│       ├── helpers.py
│       └── parsers.py
├── requirements.txt
└── .env
```

**API Endpoints:**

| Method | Endpoint | Description |
|--------|----------|-------------|
| GET | `/api/alerts` | List recent alerts |
| GET | `/api/alerts/{id}` | Get specific alert details |
| POST | `/api/investigate` | Start AI investigation |
| POST | `/api/events/ingest` | Ingest security events |
| GET | `/api/metrics` | Dashboard metrics |
| WS | `/ws` | WebSocket for real-time updates |

---

### 3. AI/ML Layer

#### Ollama (Llama 3 LLM)

**Model**: Llama 3.2 (3B parameters)

**Why Llama 3?**
- Fast inference on consumer hardware
- Strong reasoning capabilities
- Open-source and free
- Runs locally (no API costs, data privacy)

**Configuration:**
```python
# services/llm_service.py
import ollama

class LLMService:
    def __init__(self):
        self.model = "llama3.2:3b"
        self.client = ollama.Client()

    async def query(self, prompt: str, context: list[str]) -> str:
        # Format prompt with context
        full_prompt = self._format_prompt(prompt, context)

        # Call Ollama
        response = self.client.chat(
            model=self.model,
            messages=[{"role": "user", "content": full_prompt}]
        )

        return response['message']['content']
```

**Prompt Engineering:**
```
System: You are a cybersecurity analyst investigating security incidents.
Analyze the following logs and provide insights.

Context (Retrieved Logs):
{context}

User Query: {query}

Instructions:
1. Identify key security events
2. Extract IOCs (IPs, domains, hashes)
3. Assess threat level (1-10)
4. Provide recommendations

Response:
```

---

#### ChromaDB (Vector Database)

**Purpose**: Semantic search over security logs

**How it Works:**
1. **Embedding Generation**: Convert logs to 768-dim vectors using `sentence-transformers`
2. **Storage**: Store vectors in ChromaDB with metadata
3. **Query**: User query → embedding → similarity search → retrieve top-k logs
4. **Feed to LLM**: Retrieved logs used as context for Llama 3

**Implementation:**

```python
# services/vector_store.py
import chromadb
from chromadb.utils import embedding_functions

class VectorStore:
    def __init__(self):
        self.client = chromadb.PersistentClient(path="./chroma_data")

        # Use sentence-transformers for embeddings
        self.embedding_fn = embedding_functions.SentenceTransformerEmbeddingFunction(
            model_name="all-MiniLM-L6-v2"
        )

        # Create collection
        self.collection = self.client.get_or_create_collection(
            name="security_logs",
            embedding_function=self.embedding_fn
        )

    def add_event(self, event_id: str, log_text: str, metadata: dict):
        """Add security event to vector store"""
        self.collection.add(
            ids=[event_id],
            documents=[log_text],
            metadatas=[metadata]
        )

    def semantic_search(self, query: str, top_k: int = 10) -> list:
        """Search for similar logs"""
        results = self.collection.query(
            query_texts=[query],
            n_results=top_k
        )
        return results
```

**Example:**
```python
# User query: "Show me SSH brute-force attacks"
# System generates embedding for query
# Finds semantically similar logs:
# - "Failed password for invalid user admin from 185.220.101.45"
# - "Authentication failure for user root"
# - "Maximum authentication attempts exceeded"
```

---

#### LangGraph (Multi-Agent Workflows)

**Purpose**: Orchestrate complex investigation workflows

**Graph Structure:**

```
┌─────────────┐
│   START     │
│  (Query)    │
└──────┬──────┘
       │
┌──────▼──────────┐
│  Query Analysis │ ← Determine intent (alert search, IOC lookup, etc.)
└──────┬──────────┘
       │
       ├───────────────┬──────────────┬────────────────┐
       │               │              │                │
┌──────▼────────┐ ┌───▼────────┐ ┌──▼──────────┐ ┌───▼────────┐
│ Vector Search │ │ IOC Extract│ │  Wazuh API  │ │ Threat Intel│
│  (ChromaDB)   │ │   (Regex)  │ │   Query     │ │   Lookup    │
└──────┬────────┘ └───┬────────┘ └──┬──────────┘ └───┬────────┘
       │               │              │                │
       └───────────────┴──────────────┴────────────────┘
                       │
                ┌──────▼──────────┐
                │  LLM Analysis   │ ← Llama 3 synthesizes results
                │  (Llama 3)      │
                └──────┬──────────┘
                       │
                ┌──────▼──────────┐
                │   Response      │ ← Formatted output
                │  Generation     │
                └──────┬──────────┘
                       │
                ┌──────▼──────┐
                │     END      │
                └──────────────┘
```

**Implementation:**

```python
# workflows/investigation.py
from langgraph.graph import StateGraph, END

class InvestigationWorkflow:
    def __init__(self, llm_service, vector_store, wazuh_client):
        self.llm = llm_service
        self.vector_store = vector_store
        self.wazuh = wazuh_client

        # Define workflow graph
        self.graph = StateGraph()
        self.graph.add_node("analyze_query", self.analyze_query)
        self.graph.add_node("vector_search", self.vector_search)
        self.graph.add_node("extract_iocs", self.extract_iocs)
        self.graph.add_node("llm_analyze", self.llm_analyze)
        self.graph.add_node("format_response", self.format_response)

        # Define edges
        self.graph.add_edge("analyze_query", "vector_search")
        self.graph.add_edge("vector_search", "extract_iocs")
        self.graph.add_edge("extract_iocs", "llm_analyze")
        self.graph.add_edge("llm_analyze", "format_response")
        self.graph.add_edge("format_response", END)

        self.workflow = self.graph.compile()

    async def investigate(self, query: str):
        """Run investigation workflow"""
        result = await self.workflow.ainvoke({
            "query": query,
            "context": [],
            "iocs": {},
            "response": ""
        })
        return result
```

---

### 4. Data Sources

#### Wazuh SIEM Integration

**API Authentication:**
```python
# services/wazuh_client.py
import httpx

class WazuhClient:
    def __init__(self, base_url: str, username: str, password: str):
        self.base_url = base_url
        self.token = None
        self.authenticate(username, password)

    def authenticate(self, username: str, password: str):
        """Get JWT token from Wazuh API"""
        response = httpx.post(
            f"{self.base_url}/security/user/authenticate",
            auth=(username, password),
            verify=False
        )
        self.token = response.json()["data"]["token"]

    def get_alerts(self, limit: int = 100):
        """Fetch recent alerts"""
        headers = {"Authorization": f"Bearer {self.token}"}
        response = httpx.get(
            f"{self.base_url}/alerts",
            headers=headers,
            params={"limit": limit, "sort": "-timestamp"}
        )
        return response.json()["data"]["affected_items"]
```

**Event Schema:**
```json
{
  "timestamp": "2025-01-15T10:23:45.123Z",
  "agent": {
    "id": "001",
    "name": "web-server-01",
    "ip": "192.168.1.100"
  },
  "rule": {
    "id": "5710",
    "description": "sshd: Attempt to login using a non-existent user",
    "level": 5,
    "groups": ["syslog", "sshd", "authentication_failed"]
  },
  "data": {
    "srcip": "185.220.101.45",
    "srcport": "52341",
    "dstuser": "admin"
  }
}
```

---

## 🔄 Data Flow

### 1. Event Ingestion Pipeline

```
┌────────────┐      ┌──────────────┐      ┌───────────────┐
│  Wazuh     │─────▶│   FastAPI    │─────▶│   ChromaDB    │
│   SIEM     │ HTTP │   Backend    │      │  (Vectors)    │
└────────────┘      └──────┬───────┘      └───────────────┘
                           │
                           ▼
                    ┌──────────────┐
                    │  PostgreSQL  │ (Optional: for structured data)
                    └──────────────┘
```

**Steps:**
1. Wazuh agent collects logs (SSH, web server, etc.)
2. Wazuh manager processes and creates alerts
3. FastAPI backend polls Wazuh API every 30 seconds
4. Backend parses events and extracts:
   - Timestamp
   - Source IP
   - Event type
   - Severity
5. Events stored in:
   - ChromaDB (as vectors for semantic search)
   - PostgreSQL (optional: for structured queries)

**Code:**
```python
# services/event_processor.py
async def ingest_events():
    """Background task: Poll Wazuh and ingest events"""
    while True:
        # Fetch new alerts from Wazuh
        alerts = wazuh_client.get_alerts(limit=100)

        for alert in alerts:
            # Parse alert
            event = parse_alert(alert)

            # Store in ChromaDB for RAG
            vector_store.add_event(
                event_id=event.id,
                log_text=event.description,
                metadata=event.metadata
            )

            # Optionally store in PostgreSQL
            db.save_event(event)

        await asyncio.sleep(30)  # Poll every 30 seconds
```

---

### 2. AI Investigation Flow

```
User Query
    │
    ▼
┌─────────────────────────────────────────────────────────┐
│  1. QUERY ANALYSIS                                      │
│     "Show me failed SSH logins from Russia"             │
│     → Intent: Search alerts                             │
│     → Filters: auth_failed, SSH, country=RU             │
└─────────────────────┬───────────────────────────────────┘
                      ▼
┌─────────────────────────────────────────────────────────┐
│  2. VECTOR SEARCH (ChromaDB)                            │
│     Query embedding → Semantic search                   │
│     → Returns top 20 similar logs                       │
└─────────────────────┬───────────────────────────────────┘
                      ▼
┌─────────────────────────────────────────────────────────┐
│  3. IOC EXTRACTION                                      │
│     Regex parsing of logs:                              │
│     → IPs: 185.220.101.45, 103.142.24.89               │
│     → Usernames: admin, root                            │
│     → Ports: 22                                         │
└─────────────────────┬───────────────────────────────────┘
                      ▼
┌─────────────────────────────────────────────────────────┐
│  4. LLM ANALYSIS (Llama 3)                              │
│     Prompt: Analyze these logs for threats             │
│     Context: Retrieved logs + IOCs                      │
│     → LLM generates response with insights              │
└─────────────────────┬───────────────────────────────────┘
                      ▼
┌─────────────────────────────────────────────────────────┐
│  5. RESPONSE FORMATTING                                 │
│     {                                                   │
│       "summary": "Found 47 failed SSH attempts...",     │
│       "iocs": {...},                                    │
│       "recommendations": "Block IPs..."                 │
│     }                                                   │
└─────────────────────┬───────────────────────────────────┘
                      ▼
                  User receives
                  formatted response
```

---

## 🔐 Security Considerations

### Authentication & Authorization (Future)

```python
# core/security.py
from fastapi import Depends, HTTPException, status
from fastapi.security import OAuth2PasswordBearer

oauth2_scheme = OAuth2PasswordBearer(tokenUrl="token")

async def get_current_user(token: str = Depends(oauth2_scheme)):
    """Verify JWT token"""
    # Decode and validate token
    # Return user object
    pass
```

### API Rate Limiting

```python
from slowapi import Limiter, _rate_limit_exceeded_handler

limiter = Limiter(key_func=get_remote_address)
app.state.limiter = limiter

@app.get("/api/investigate")
@limiter.limit("10/minute")
async def investigate(request: Request, query: str):
    # Investigation logic
    pass
```

### Input Validation

- All inputs validated with Pydantic models
- SQL injection prevention (using ORMs)
- XSS prevention (React escapes by default)
- CORS configured for specific origins

---

## 📈 Scalability

### Current Architecture (Single Server)

**Limitations:**
- Handles ~10,000 events/hour
- Single point of failure
- Limited to one Ollama instance

### Future Improvements

#### 1. Horizontal Scaling

```
                    ┌─────────────────┐
                    │  Load Balancer  │
                    └────────┬────────┘
                             │
          ┌──────────────────┼──────────────────┐
          │                  │                  │
    ┌─────▼─────┐     ┌──────▼────┐     ┌──────▼────┐
    │  FastAPI  │     │  FastAPI  │     │  FastAPI  │
    │  Instance │     │  Instance │     │  Instance │
    └─────┬─────┘     └──────┬────┘     └──────┬────┘
          │                  │                  │
          └──────────────────┼──────────────────┘
                             │
                    ┌────────▼────────┐
                    │  Shared         │
                    │  ChromaDB       │
                    └─────────────────┘
```

#### 2. Message Queue (Redis/RabbitMQ)

- Decouple event ingestion from processing
- Asynchronous task processing
- Better handling of traffic spikes

#### 3. Caching Layer (Redis)

```python
# Cache frequent queries
@cache(expire=300)  # 5-minute cache
async def get_alerts():
    return await wazuh_client.get_alerts()
```

---

## 🧪 Testing Strategy

### Unit Tests
```python
# tests/test_llm_service.py
import pytest
from app.services.llm_service import LLMService

@pytest.mark.asyncio
async def test_llm_query():
    llm = LLMService()
    response = await llm.query(
        prompt="Analyze this SSH login failure",
        context=["Failed password for admin from 1.2.3.4"]
    )
    assert "SSH" in response or "login" in response
```

### Integration Tests
```python
# tests/test_investigation_workflow.py
@pytest.mark.asyncio
async def test_investigation_workflow():
    workflow = InvestigationWorkflow(llm, vector_store, wazuh)
    result = await workflow.investigate("Show failed logins")
    assert result["iocs"] is not None
    assert len(result["logs"]) > 0
```

### API Tests
```python
# tests/test_api.py
from fastapi.testclient import TestClient

client = TestClient(app)

def test_investigate_endpoint():
    response = client.post(
        "/api/investigate",
        json={"query": "Show SSH attacks"}
    )
    assert response.status_code == 200
    assert "iocs" in response.json()
```

---

## 📊 Monitoring & Observability

### Logging

```python
# core/logging.py
import logging
import structlog

structlog.configure(
    processors=[
        structlog.processors.TimeStamper(fmt="iso"),
        structlog.processors.JSONRenderer()
    ]
)

logger = structlog.get_logger()

# Usage
logger.info("event_ingested", event_id=123, source="wazuh")
```

### Metrics (Prometheus)

```python
from prometheus_client import Counter, Histogram

# Define metrics
events_processed = Counter('events_processed_total', 'Total events processed')
query_duration = Histogram('query_duration_seconds', 'Query processing time')

# Use in code
events_processed.inc()
with query_duration.time():
    result = await investigate(query)
```

---

## 🗺️ Future Architecture Enhancements

1. **Distributed Tracing** (Jaeger/OpenTelemetry)
2. **Service Mesh** (Istio) for microservices
3. **Container Orchestration** (Kubernetes)
4. **CI/CD Pipeline** (GitHub Actions, ArgoCD)
5. **Infrastructure as Code** (Terraform, Ansible)

---

**This architecture is designed to be modular, scalable, and production-ready.**

Return to [README.md](./README.md)
