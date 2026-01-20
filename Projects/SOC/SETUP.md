# 🛠️ SOC AI Dashboard - Complete Setup Guide

This guide will walk you through setting up the entire AI-Powered SOC Dashboard from scratch.

**Estimated Setup Time**: 4-6 hours
**Difficulty**: Intermediate
**Prerequisites**: Basic Linux, networking, and command-line knowledge

---

## 📋 Table of Contents

1. [Hardware Requirements](#hardware-requirements)
2. [Part 1: Proxmox Environment Setup](#part-1-proxmox-environment-setup)
3. [Part 2: Wazuh SIEM Installation](#part-2-wazuh-siem-installation)
4. [Part 3: Backend Setup](#part-3-backend-setup)
5. [Part 4: Frontend Setup](#part-4-frontend-setup)
6. [Part 5: Attack Simulation (Optional)](#part-5-attack-simulation-optional)
7. [Troubleshooting](#troubleshooting)
8. [Next Steps](#next-steps)

---

## Hardware Requirements

### Minimum Specs:
- **CPU**: 4 cores (8 threads recommended)
- **RAM**: 16GB (8GB for VMs, 8GB for host)
- **Storage**: 100GB free space
- **Network**: Wired connection recommended

### Recommended Specs:
- **CPU**: 6+ cores
- **RAM**: 32GB
- **Storage**: 250GB SSD
- **GPU**: Optional (for faster LLM inference)

---

## Part 1: Proxmox Environment Setup

### Step 1: Install Proxmox VE

**Option A: Bare Metal Installation**
1. Download Proxmox VE ISO: https://www.proxmox.com/en/downloads
2. Create bootable USB (use Rufus on Windows, `dd` on Linux)
3. Boot from USB and follow installation wizard
4. Set static IP address during installation
5. Access web UI: `https://YOUR-PROXMOX-IP:8006`

**Option B: Alternative Hypervisor**
- Can use VMware, VirtualBox, or Hyper-V
- Ensure nested virtualization is enabled
- May need to adjust VM settings for performance

### Step 2: Configure Proxmox Networking

1. **Access Proxmox Web UI**
   ```
   URL: https://YOUR-PROXMOX-IP:8006
   Default user: root
   Password: (set during installation)
   ```

2. **Create Internal Network (for lab isolation)**
   ```bash
   # On Proxmox host shell
   nano /etc/network/interfaces
   ```

   Add:
   ```
   auto vmbr1
   iface vmbr1 inet static
       address 192.168.100.1/24
       bridge-ports none
       bridge-stp off
       bridge-fd 0
   ```

3. **Apply network changes**
   ```bash
   ifreload -a
   ```

### Step 3: Upload ISO Images

1. Navigate to **Storage** → **local** → **ISO Images**
2. Download and upload:
   - **Ubuntu Server 22.04 LTS**: https://ubuntu.com/download/server
   - **Kali Linux** (optional): https://www.kali.org/get-kali/#kali-virtual-machines

---

## Part 2: Wazuh SIEM Installation

### Step 4: Create Wazuh VM

1. **In Proxmox, click "Create VM"**
   - **VM ID**: 100
   - **Name**: wazuh-siem
   - **OS**: Ubuntu Server 22.04 ISO
   - **Disk**: 50GB
   - **CPU**: 2 cores
   - **RAM**: 4096MB
   - **Network**: vmbr0 (bridged to internet)

2. **Start VM and access console**

### Step 5: Install Ubuntu Server

1. Follow Ubuntu installation wizard
   - **Name**: wazuh-siem
   - **Username**: admin
   - **Password**: (your choice)
   - **Install OpenSSH**: ✅ Yes

2. Update system after installation:
   ```bash
   sudo apt update && sudo apt upgrade -y
   ```

### Step 6: Install Wazuh All-in-One

Run the official Wazuh installation script:

```bash
# Install dependencies
sudo apt install curl gnupg2 apt-transport-https -y

# Download and run Wazuh installation script
curl -sO https://packages.wazuh.com/4.7/wazuh-install.sh
sudo bash ./wazuh-install.sh -a
```

**Installation takes 10-15 minutes. You'll see:**
```
INFO: --- Summary ---
INFO: Wazuh dashboard: https://192.168.X.X
INFO: User: admin
INFO: Password: <SAVE THIS PASSWORD>
INFO: Installation finished.
```

**⚠️ IMPORTANT: Save the admin password shown at the end!**

### Step 7: Access Wazuh Dashboard

1. Open browser: `https://WAZUH-VM-IP`
2. Accept the self-signed certificate warning
3. Login with:
   - **Username**: admin
   - **Password**: (from installation output)

---

## Part 3: Backend Setup

### Step 8: Create Development VM (or use your host)

**Option A: Create new Ubuntu VM in Proxmox**
- Follow same steps as Wazuh VM
- **Name**: dev-server
- **Disk**: 30GB
- **CPU**: 2 cores
- **RAM**: 4096MB

**Option B: Use your local machine**
- Requires Linux, macOS, or WSL2 on Windows
- Python 3.11+ required

### Step 9: Install System Dependencies

```bash
# Update system
sudo apt update && sudo apt upgrade -y

# Install Python 3.11+
sudo apt install python3 python3-pip python3-venv -y

# Install Node.js (for frontend later)
curl -fsSL https://deb.nodesource.com/setup_20.x | sudo -E bash -
sudo apt install nodejs -y

# Install Git
sudo apt install git -y

# Verify installations
python3 --version  # Should be 3.11+
node --version     # Should be v20+
npm --version
```

### Step 10: Install Ollama & Llama 3

```bash
# Install Ollama
curl -fsSL https://ollama.com/install.sh | sh

# Verify installation
ollama --version

# Pull Llama 3 model (3B parameter version for faster inference)
ollama pull llama3.2:3b

# Test Ollama
ollama run llama3.2:3b "Hello, are you working?"
```

**Note**: First model download takes 5-10 minutes (2GB+ download)

### Step 11: Clone Repository

```bash
cd ~
git clone https://github.com/yourusername/soc-ai-dashboard.git
cd soc-ai-dashboard
```

### Step 12: Backend Setup

```bash
# Navigate to backend folder
cd backend

# Create virtual environment
python3 -m venv venv

# Activate virtual environment
source venv/bin/activate  # Linux/Mac
# OR
venv\Scripts\activate  # Windows

# Install Python dependencies
pip install --upgrade pip
pip install -r requirements.txt
```

**Create `backend/requirements.txt` if it doesn't exist:**
```txt
fastapi==0.109.0
uvicorn[standard]==0.27.0
langchain==0.1.6
langgraph==0.0.20
chromadb==0.4.22
ollama==0.1.6
websockets==12.0
python-dotenv==1.0.0
pydantic==2.5.3
httpx==0.26.0
requests==2.31.0
```

### Step 13: Configure Environment Variables

```bash
# Create .env file
cd ~/soc-ai-dashboard/backend
nano .env
```

Add:
```env
# Wazuh Configuration
WAZUH_API_URL=https://WAZUH-VM-IP:55000
WAZUH_API_USERNAME=admin
WAZUH_API_PASSWORD=your-wazuh-password

# Ollama Configuration
OLLAMA_BASE_URL=http://localhost:11434
OLLAMA_MODEL=llama3.2:3b

# ChromaDB Configuration
CHROMA_PERSIST_DIRECTORY=./chroma_data

# FastAPI Configuration
API_HOST=0.0.0.0
API_PORT=8000
DEBUG=True
```

### Step 14: Initialize ChromaDB

ChromaDB will auto-initialize on first run, but you can test it:

```bash
# Start Python in virtual environment
python3

# Test ChromaDB
>>> import chromadb
>>> client = chromadb.Client()
>>> print("ChromaDB working!")
>>> exit()
```

### Step 15: Start Backend Server

```bash
cd ~/soc-ai-dashboard/backend
source venv/bin/activate

# Start FastAPI server
uvicorn app.main:app --reload --host 0.0.0.0 --port 8000
```

**You should see:**
```
INFO: Uvicorn running on http://0.0.0.0:8000
INFO: Application startup complete.
```

**Test the API:**
- Open browser: `http://localhost:8000/docs`
- You should see the FastAPI interactive documentation

---

## Part 4: Frontend Setup

### Step 16: Frontend Installation

Open a **new terminal** (keep backend running):

```bash
# Navigate to frontend folder
cd ~/soc-ai-dashboard/frontend

# Install dependencies
npm install

# Install additional packages
npm install recharts axios react-router-dom @tanstack/react-query
npm install -D tailwindcss postcss autoprefixer
npx tailwindcss init -p
```

### Step 17: Configure Frontend Environment

```bash
# Create .env file
nano .env
```

Add:
```env
REACT_APP_API_URL=http://localhost:8000
REACT_APP_WS_URL=ws://localhost:8000/ws
```

### Step 18: Start Frontend

```bash
# Start React development server
npm start
```

**Browser should automatically open to `http://localhost:3000`**

---

## Part 5: Attack Simulation (Optional)

### Step 19: Create Kali Linux VM

1. **In Proxmox, click "Create VM"**
   - **VM ID**: 102
   - **Name**: kali-attacker
   - **OS**: Kali Linux ISO
   - **Disk**: 30GB
   - **CPU**: 2 cores
   - **RAM**: 2048MB
   - **Network**: vmbr1 (internal network)

2. **Start Kali VM**
   - Default credentials: `kali / kali`
   - Update: `sudo apt update && sudo apt upgrade -y`

### Step 20: Install Wazuh Agent on Target

Create a victim VM (Ubuntu Server) and install Wazuh agent:

```bash
# On victim VM
curl -s https://packages.wazuh.com/key/GPG-KEY-WAZUH | sudo gpg --no-default-keyring --keyring gnupg-ring:/usr/share/keyrings/wazuh.gpg --import
echo "deb [signed-by=/usr/share/keyrings/wazuh.gpg] https://packages.wazuh.com/4.x/apt/ stable main" | sudo tee /etc/apt/sources.list.d/wazuh.list

sudo apt update
sudo apt install wazuh-agent -y

# Configure agent to connect to Wazuh manager
sudo nano /var/ossec/etc/ossec.conf

# Find <address> tag and set to your Wazuh VM IP:
# <address>192.168.X.X</address>

# Start agent
sudo systemctl enable wazuh-agent
sudo systemctl start wazuh-agent
```

### Step 21: Generate Test Attacks

From Kali Linux:

```bash
# SSH brute-force simulation
hydra -l admin -P /usr/share/wordlists/rockyou.txt ssh://VICTIM-IP

# Port scan
nmap -sS -p- VICTIM-IP

# Web enumeration
nikto -h http://VICTIM-IP
```

Check Wazuh dashboard to see alerts generated!

---

## Troubleshooting

### Wazuh Issues

**Problem**: Can't access Wazuh dashboard
```bash
# Check Wazuh services
sudo systemctl status wazuh-manager
sudo systemctl status wazuh-indexer
sudo systemctl status wazuh-dashboard

# View logs
sudo tail -f /var/ossec/logs/ossec.log
```

**Problem**: Agent not connecting
```bash
# On agent VM
sudo systemctl status wazuh-agent
sudo tail -f /var/ossec/logs/ossec.log

# Check connectivity
ping WAZUH-MANAGER-IP
telnet WAZUH-MANAGER-IP 1514
```

### Ollama Issues

**Problem**: Ollama not responding
```bash
# Check if Ollama is running
curl http://localhost:11434/api/tags

# Restart Ollama
sudo systemctl restart ollama

# Check logs
journalctl -u ollama -f
```

**Problem**: Model too slow
- Use smaller model: `ollama pull llama3.2:1b`
- Update `.env`: `OLLAMA_MODEL=llama3.2:1b`

### Backend Issues

**Problem**: FastAPI won't start
```bash
# Check Python version
python3 --version  # Must be 3.11+

# Reinstall dependencies
pip install --upgrade -r requirements.txt

# Check for errors
uvicorn app.main:app --reload --log-level debug
```

**Problem**: Can't connect to Wazuh API
```bash
# Test Wazuh API manually
curl -k -u admin:PASSWORD https://WAZUH-IP:55000/

# Check .env file has correct credentials
cat .env
```

### Frontend Issues

**Problem**: npm install fails
```bash
# Clear npm cache
npm cache clean --force

# Delete node_modules and reinstall
rm -rf node_modules package-lock.json
npm install
```

**Problem**: Can't connect to backend
- Verify backend is running: `http://localhost:8000/docs`
- Check `.env` has correct `REACT_APP_API_URL`
- Check browser console for CORS errors

### Network Issues

**Problem**: VMs can't reach internet
```bash
# On Proxmox host, check NAT is enabled
iptables -t nat -L

# Enable IP forwarding
echo 1 > /proc/sys/net/ipv4/ip_forward

# Add NAT rule
iptables -t nat -A POSTROUTING -o vmbr0 -j MASQUERADE
```

---

## Next Steps

### ✅ Verification Checklist

- [ ] Wazuh dashboard accessible and showing data
- [ ] Ollama responding to queries (`ollama list` shows models)
- [ ] Backend API docs available at `http://localhost:8000/docs`
- [ ] Frontend loading at `http://localhost:3000`
- [ ] ChromaDB initialized (check `backend/chroma_data/` folder exists)
- [ ] Wazuh agents sending logs (visible in dashboard)

### 🚀 What's Next?

1. **Ingest Security Events**
   - Configure Wazuh to forward logs to FastAPI backend
   - Set up event processing pipeline

2. **Test AI Features**
   - Try natural language queries
   - Test RAG-based threat intelligence
   - Run investigation workflows

3. **Add More Agents**
   - Install Wazuh agents on more VMs
   - Generate diverse security events

4. **Customize Dashboard**
   - Add custom visualizations
   - Create alert widgets
   - Build investigation timeline

5. **Document Your Work**
   - Take screenshots
   - Record demo video
   - Update README with your setup

---

## 📚 Additional Resources

### Official Documentation:
- Wazuh: https://documentation.wazuh.com/
- Ollama: https://ollama.com/docs
- FastAPI: https://fastapi.tiangolo.com/
- React: https://react.dev/
- LangChain: https://python.langchain.com/

### Useful Commands Cheat Sheet:

```bash
# Wazuh
sudo systemctl status wazuh-manager
sudo tail -f /var/ossec/logs/ossec.log

# Ollama
ollama list
ollama run llama3.2:3b
curl http://localhost:11434/api/tags

# Backend
source venv/bin/activate
uvicorn app.main:app --reload
pip list

# Frontend
npm start
npm run build
npm install <package>

# Proxmox VMs
qm list              # List all VMs
qm start 100         # Start VM ID 100
qm stop 100          # Stop VM ID 100
qm status 100        # Check VM status
```

---

## 🆘 Getting Help

If you encounter issues not covered here:

1. **Check logs** - Most issues can be diagnosed from log files
2. **Google the error** - Someone has likely faced the same issue
3. **GitHub Issues** - Open an issue in the repository
4. **Community forums**:
   - Wazuh: https://groups.google.com/g/wazuh
   - Proxmox: https://forum.proxmox.com/
   - LangChain: https://github.com/langchain-ai/langchain/discussions

---

**Setup complete! You now have a fully functional AI-powered SOC dashboard.**

Return to [README.md](./README.md) for usage instructions.
