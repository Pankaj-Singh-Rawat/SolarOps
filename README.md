# SolarOps: GitOps-Driven Resilient Micro-Grid Solar Optimizer

## 1. Project Overview & Problem Statement

### 1.1 Problem Statement
Traditional residential and light-commercial hybrid solar configurations rely on rigid, static threshold switching logic to manage power routing between solar arrays, local battery storage, and the utility grid. This lack of dynamic adaptability causes significant operational inefficiencies:
* **Suboptimal Energy Utilization:** Systems fail to account for real-time household consumption spikes or variations in solar irradiance, leading to unnecessary grid-draw or wasted solar generation.
* **Accelerated Asset Degradation:** Improper battery charge/discharge cycles based on fixed parameters accelerate battery capacity fade.
* **Infrastructure Management Gaps:** Conventional IoT micro-grids suffer from fragmented deployment pipelines, lack of containerized portability, and zero automated fail-safe redundancies when network connections drop.

### 1.2 Project Solution
`SolarOps` is an enterprise-grade, resilient, micro-grid optimization framework that bridges low-power wireless edge sensing with modern DevOps infrastructure practices. 

Engineered to target the **Silicon Labs SiWx91x** Wi-Fi connectivity family via the **WiSeConnect SDK**, the system tracks real-time solar generation and household load metrics to compute optimal power routing schedules. Built entirely on an open-source, containerized stack running inside Docker, `SolarOps` introduces a robust edge-autonomy fallback algorithm to guarantee continuous hardware operation during network blackouts, an automated self-healing backend configuration that automatically recovers from system crashes, and an advanced Machine Learning engine that forecasts tomorrow's power metrics.

---

## 2. System Architecture & Requirements

### 2.1 Technical Requirements
* **Hardware:** Silicon Labs SiWx91x Dual-Core Wi-Fi Development Kit (or simulated edge node target)
* **Software Toolchain:** Simplicity Studio 5 & WiSeConnect SDK v3.x
* **Infrastructure Stack:** Docker Compose, Eclipse Mosquitto (MQTT), Prometheus, Grafana, Streamlit, Flask

### 2.2 System Block Diagram

```text
+-----------------------------------------------------------------------+
|                    SILICON LABS EDGE LAYER (SiWx91x)                  |
|                                                                       |
|   +-------------------+       +-------------------+                   |
|   |    LDR Sensor     |       |   Potentiometer   |                   |
|   | (Solar Intensity) |       | (Household Load)  |                   |
|   +---------+---------+       +---------+---------+                   |
|             |                           |                             |
|             +-------------+-------------+                             |
|                           |                                           |
|                           v                                           |
|               +-----------------------+                               |
|               | Silicon Labs SiWx91x  |                               |
|               |  (WiSeConnect SDK v3) |                               |
|               |                       |                               |
|               | * Local State Machine |                               |
|               |   Autonomy Algorithm  |                               |
|               +-----------+-----------+                               |
|                           |                                           |
|                           | (Relay Control Pins)                      |
|                           v                                           |
|               +-----------------------+                               |
|               |  5V Relay / Status    |                               |
|               |  LED indicators       |                               |
|               +-----------+-----------+                               |
+---------------------------|-------------------------------------------+
                            |
                            | (MQTT Secure Protocol over Local Wi-Fi)
                            v
+-----------------------------------------------------------------------+
|                  DEVOPS & BACKEND LAYER (Local Laptop)                |
|                        Managed by Docker Compose                      |
|                                                                       |
|   +-------------------+    +-------------------+    +--------------+  |
|   | Eclipse Mosquitto |    |Streamlit Dashboard|    |Predict.py(ML)|  |
|   |   (MQTT Broker)   |--->|   (Core Logic)    |<-->| (scikit-learn|  |
|   +---------+---------+    +---------+---------+    +--------------+  |
|             |                        ^                                |
|             v                        | (Auto-Heal)                    |
|   +-------------------+              |                                |
|   |Prometheus/Grafana |--------------+                                |
|   | (Metrics/Charts)  |                                               |
|   +-------------------+                                               |
+-----------------------------------------------------------------------+
```

## 3. Working Methodology / Flow of Project

The framework executes on a continuous four-stage loop: **Ingestion**, **Orchestration**, **Analytics**, and **Execution**, completely backed by a live, automated infrastructure monitoring system.

### 3.1 Data Ingestion Phase
The **SiWx91x** edge microcontroller runs a persistent sampling loop, polling the **LDR sensor** (representing solar panel generation) and the **Potentiometer** (representing household electrical load) every 2 seconds.

Under normal operating conditions, these metrics are packaged into a structured `JSON` payload and transmitted via the native **WiSeConnect SDK** wireless stack to an **Eclipse Mosquitto** MQTT Broker running inside an isolated Docker container network.

### 3.2 Resilient Orchestration & Analytics Phase
* **Decision Matrix:** The containerized `Streamlit` web application reads data from the broker. If `Solar Input` > `House Load`, it commands the system to route power to the home via the solar array. If the load spikes beyond generation thresholds, it prepares to shift safely to Grid Power.
* **Predictive AI Engine:** A dedicated Python machine learning container (`predict.py`) fetches tomorrow's weather forecast data via a free API, merges it with historical generation parameters, and processes it through a linear inference model to output tomorrow's expected solar energy production forecast.
* **Emergency Alert Trigger:** If the hardware node stops streaming telemetry strings for more than a critical 10-second timeout window, the backend infers a backhaul blackout and fires an automated HTTP Webhook notification alert straight to the engineer's mobile device.

### 3.3 The DevOps Self-Healing Loop
* **Continuous Observability:** System-level telemetry metrics are pulled continuously by `Prometheus` and visualized inside a `Grafana` dashboard to monitor container uptimes, RAM consumption, and network connection drops.
* **Automated Container Recovery:** The core application container uses a native Docker daemon `healthcheck`. If the dashboard execution engine crashes or hangs, Docker catches the failure instantly and automatically instantiates a fresh, healthy container replica within 5 seconds without manual system administration.

### 3.4 Hardware Execution & Fail-Safe Autonomy
* **Actuation:** The microcontroller receives incoming routing command vectors from the dashboard via MQTT and switches a physical **5V Relay Module** to alternate the home's active power lanes.
* **The Local Autonomy Fallback:** If the room's Wi-Fi drops out entirely, the embedded firmware intercepts the network timeout via the **WiSeConnect API**. It instantly breaks out of the online routine and shifts into an autonomous local state machine—calculating grid pathing directly on-chip via raw analog sensor pins to ensure zero power grid blackouts.

---

## 4. Current Progress Status & Future Scope

### 4.1 Completed Milestones
* [x] Formulated complete architectural design blueprint targeting Silicon Labs ecosystems.
* [x] Configured declarative `docker-compose.yml` multi-container architecture.
* [x] Developed modular Python Streamlit logic dashboard templates.
* [x] Programmed containerized scikit-learn Flask prediction service (`predict.py`).
* [x] Created structural automation workflow templates via GitHub Actions CI.

### 4.2 Future Scope
* **Silicon Labs Native Board Implementation:** Deploying the optimized application image directly onto the physical **SiWx91x radio boards** to validate power optimization parameters in real-world environments.
* **Advanced Fleet Observability:** Expanding the Prometheus metrics scraper to log wireless signal strength (`RSSI`), packet loss indices, and granular container execution times across large-scale hardware deployments.