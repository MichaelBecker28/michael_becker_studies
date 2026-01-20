# 📁 Project Structure Guide

This document outlines the complete folder and file structure for the SOC AI Dashboard project.

---

## 🌳 Complete Directory Tree

```
soc-ai-dashboard/
│
├── README.md                          # Main project overview
├── SETUP.md                           # Installation instructions
├── ARCHITECTURE.md                    # System architecture documentation
├── FEATURES.md                        # Feature documentation
├── RESUME_CONTENT.md                  # Resume/portfolio content
├── PROJECT_STRUCTURE.md               # This file
├── .gitignore                         # Git ignore rules
├── LICENSE                            # MIT License
│
├── backend/                           # Python FastAPI backend
│   ├── app/
│   │   ├── __init__.py
│   │   ├── main.py                    # FastAPI app entry point
│   │   │
│   │   ├── api/                       # API layer
│   │   │   ├── __init__.py
│   │   │   ├── routes/
│   │   │   │   ├── __init__.py
│   │   │   │   ├── alerts.py          # Alert endpoints
│   │   │   │   ├── investigate.py     # Investigation endpoints
│   │   │   │   ├── events.py          # Event ingestion
│   │   │   │   └── websocket.py       # WebSocket handlers
│   │   │   └── dependencies.py        # Dependency injection
│   │   │
│   │   ├── core/                      # Core configuration
│   │   │   ├── __init__.py
│   │   │   ├── config.py              # Settings & env vars
│   │   │   ├── security.py            # Auth & security
│   │   │   └── logging.py             # Logging setup
│   │   │
│   │   ├── services/                  # Business logic
│   │   │   ├── __init__.py
│   │   │   ├── wazuh_client.py        # Wazuh API client
│   │   │   ├── llm_service.py         # Ollama/LLM wrapper
│   │   │   ├── vector_store.py        # ChromaDB operations
│   │   │   └── event_processor.py     # Log parsing
│   │   │
│   │   ├── workflows/                 # LangGraph workflows
│   │   │   ├── __init__.py
│   │   │   ├── investigation.py       # Investigation graph
│   │   │   ├── triage.py              # Alert triage
│   │   │   └── ioc_extraction.py      # IOC extraction
│   │   │
│   │   ├── models/                    # Pydantic models
│   │   │   ├── __init__.py
│   │   │   ├── event.py               # Event schema
│   │   │   ├── alert.py               # Alert schema
│   │   │   └── investigation.py       # Investigation schema
│   │   │
│   │   └── utils/                     # Utilities
│   │       ├── __init__.py
│   │       ├── helpers.py             # Helper functions
│   │       └── parsers.py             # Log parsers
│   │
│   ├── tests/                         # Unit & integration tests
│   │   ├── __init__.py
│   │   ├── test_api.py
│   │   ├── test_llm_service.py
│   │   ├── test_vector_store.py
│   │   └── test_workflows.py
│   │
│   ├── requirements.txt               # Python dependencies
│   ├── .env.example                   # Example environment file
│   └── README.md                      # Backend-specific docs
│
├── frontend/                          # React frontend
│   ├── public/
│   │   ├── index.html
│   │   ├── favicon.ico
│   │   └── manifest.json
│   │
│   ├── src/
│   │   ├── components/                # React components
│   │   │   ├── Dashboard/
│   │   │   │   ├── AlertsWidget.tsx
│   │   │   │   ├── MetricsCard.tsx
│   │   │   │   └── ThreatMap.tsx
│   │   │   │
│   │   │   ├── Investigation/
│   │   │   │   ├── ChatInterface.tsx
│   │   │   │   ├── Timeline.tsx
│   │   │   │   └── IOCExtractor.tsx
│   │   │   │
│   │   │   └── Shared/
│   │   │       ├── Navbar.tsx
│   │   │       ├── Loader.tsx
│   │   │       └── ErrorBoundary.tsx
│   │   │
│   │   ├── services/                  # API clients
│   │   │   ├── api.ts                 # HTTP client (Axios)
│   │   │   └── websocket.ts           # WebSocket manager
│   │   │
│   │   ├── hooks/                     # Custom React hooks
│   │   │   ├── useWebSocket.ts
│   │   │   ├── useAlerts.ts
│   │   │   └── useInvestigation.ts
│   │   │
│   │   ├── types/                     # TypeScript types
│   │   │   └── index.ts
│   │   │
│   │   ├── utils/                     # Utility functions
│   │   │   └── formatters.ts
│   │   │
│   │   ├── styles/                    # Global styles
│   │   │   └── globals.css
│   │   │
│   │   ├── App.tsx                    # Main app component
│   │   ├── index.tsx                  # Entry point
│   │   └── setupTests.ts              # Test configuration
│   │
│   ├── package.json                   # Node dependencies
│   ├── tsconfig.json                  # TypeScript config
│   ├── tailwind.config.js             # TailwindCSS config
│   ├── .env.example                   # Example env file
│   └── README.md                      # Frontend-specific docs
│
├── docs/                              # Additional documentation
│   ├── API.md                         # API reference
│   ├── DEPLOYMENT.md                  # Deployment guide
│   ├── CONTRIBUTING.md                # Contribution guidelines
│   └── images/                        # Documentation images
│       ├── architecture-diagram.png
│       └── dashboard-screenshot.png
│
├── scripts/                           # Utility scripts
│   ├── setup.sh                       # Setup automation
│   ├── seed_data.py                   # Seed test data
│   └── backup.sh                      # Backup script
│
├── .github/                           # GitHub-specific files
│   ├── workflows/                     # GitHub Actions
│   │   ├── backend-tests.yml          # Backend CI
│   │   ├── frontend-tests.yml         # Frontend CI
│   │   └── deploy.yml                 # Deployment automation
│   │
│   └── ISSUE_TEMPLATE/                # Issue templates
│       ├── bug_report.md
│       └── feature_request.md
│
├── docker/                            # Docker configuration
│   ├── backend.Dockerfile
│   ├── frontend.Dockerfile
│   └── docker-compose.yml
│
└── chroma_data/                       # ChromaDB storage (gitignored)
    └── (vector database files)
```

---

## 📝 File Descriptions

### Root Level Files

| File | Purpose |
|------|---------|
| `README.md` | Main project overview, features, quick start |
| `SETUP.md` | Detailed installation and setup instructions |
| `ARCHITECTURE.md` | System design, architecture, data flow |
| `FEATURES.md` | Comprehensive feature documentation |
| `RESUME_CONTENT.md` | Ready-to-use resume/portfolio snippets |
| `.gitignore` | Files to exclude from version control |
| `LICENSE` | MIT License (or your choice) |

---

## 🚀 Getting Started

### 1. Create This Structure

You can create the basic structure with these commands:

```bash
# Navigate to SOC project folder
cd "C:\Git Repository\michael_becker_studies\Projects\SOC"

# Create backend structure
mkdir -p backend/app/{api/routes,core,services,workflows,models,utils}
mkdir -p backend/tests

# Create frontend structure
mkdir -p frontend/src/{components/{Dashboard,Investigation,Shared},services,hooks,types,utils,styles}
mkdir -p frontend/public

# Create docs folder
mkdir -p docs/images

# Create scripts folder
mkdir -p scripts

# Create GitHub workflows
mkdir -p .github/{workflows,ISSUE_TEMPLATE}

# Create docker folder
mkdir -p docker
```

### 2. Initialize Files

**Backend:**
```bash
# Create __init__.py files
touch backend/app/__init__.py
touch backend/app/api/__init__.py
touch backend/app/api/routes/__init__.py
touch backend/app/core/__init__.py
touch backend/app/services/__init__.py
touch backend/app/workflows/__init__.py
touch backend/app/models/__init__.py
touch backend/app/utils/__init__.py
touch backend/tests/__init__.py

# Create requirements.txt
touch backend/requirements.txt

# Create .env.example
touch backend/.env.example
```

**Frontend:**
```bash
# Initialize React app (or use create-react-app)
cd frontend
npx create-react-app . --template typescript

# Or manually create files
touch frontend/src/App.tsx
touch frontend/src/index.tsx
touch frontend/package.json
touch frontend/tsconfig.json
touch frontend/.env.example
```

---

## 📦 Essential Dependencies

### Backend Requirements (`backend/requirements.txt`)

```txt
# Web Framework
fastapi==0.109.0
uvicorn[standard]==0.27.0

# AI/ML
langchain==0.1.6
langgraph==0.0.20
chromadb==0.4.22
ollama==0.1.6
sentence-transformers==2.3.1

# HTTP & WebSockets
httpx==0.26.0
websockets==12.0
requests==2.31.0

# Data & Validation
pydantic==2.5.3
python-dotenv==1.0.0

# Testing
pytest==7.4.3
pytest-asyncio==0.21.1
```

### Frontend Dependencies (`frontend/package.json`)

```json
{
  "dependencies": {
    "react": "^18.2.0",
    "react-dom": "^18.2.0",
    "react-router-dom": "^6.21.0",
    "axios": "^1.6.5",
    "recharts": "^2.10.3",
    "@tanstack/react-query": "^5.17.9",
    "socket.io-client": "^4.6.1"
  },
  "devDependencies": {
    "@types/react": "^18.2.0",
    "@types/react-dom": "^18.2.0",
    "typescript": "^5.3.3",
    "tailwindcss": "^3.4.1",
    "autoprefixer": "^10.4.17",
    "postcss": "^8.4.33"
  }
}
```

---

## 🔐 Environment Variables

### Backend `.env.example`

```env
# Wazuh Configuration
WAZUH_API_URL=https://YOUR-WAZUH-IP:55000
WAZUH_API_USERNAME=admin
WAZUH_API_PASSWORD=your-password

# Ollama Configuration
OLLAMA_BASE_URL=http://localhost:11434
OLLAMA_MODEL=llama3.2:3b

# ChromaDB Configuration
CHROMA_PERSIST_DIRECTORY=./chroma_data

# FastAPI Configuration
API_HOST=0.0.0.0
API_PORT=8000
DEBUG=True
CORS_ORIGINS=http://localhost:3000

# Optional: Database
DATABASE_URL=postgresql://user:pass@localhost/soc_db
```

### Frontend `.env.example`

```env
REACT_APP_API_URL=http://localhost:8000
REACT_APP_WS_URL=ws://localhost:8000/ws
REACT_APP_ENVIRONMENT=development
```

---

## ✅ Recommended Next Steps

1. **✅ Initialize Git** (if not already done)
   ```bash
   git init
   git add .
   git commit -m "Initial commit: Project documentation and structure"
   ```

2. **Create GitHub Repository**
   - Go to GitHub.com
   - Create new repository: `soc-ai-dashboard`
   - Push your code:
     ```bash
     git remote add origin https://github.com/yourusername/soc-ai-dashboard.git
     git branch -M main
     git push -u origin main
     ```

3. **Set Up Backend**
   - Follow instructions in `SETUP.md`
   - Install Python dependencies
   - Configure `.env` file

4. **Set Up Frontend**
   - Follow instructions in `SETUP.md`
   - Install Node dependencies
   - Configure `.env` file

5. **Start Development**
   - Run backend: `uvicorn app.main:app --reload`
   - Run frontend: `npm start`

---

## 📚 Additional Resources

- **GitHub Project Board**: Create issues and track progress
- **Wiki**: Document design decisions and lessons learned
- **Releases**: Tag versions as you complete milestones
- **GitHub Actions**: Automate testing and deployment

---

**Happy coding! 🚀**

Return to [README.md](./README.md)
