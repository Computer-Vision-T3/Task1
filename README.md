# Computer Vision Task 1 - Image Processing Application

A C++ Desktop Application built with **Qt5** (Frontend UI) and **OpenCV** (Backend Image Processing). This tool is designed to apply various image processing algorithms, featuring a dynamic UI that adjusts layouts and parameters based on the selected operation.

## 👥 Team & Task Distribution
The 10 requirements are divided across 5 backend modules.
* **Module 1 (Tasks 1 & 2):** Additive Noise & Low Pass Filters (Average, Gaussian, Median).
* **Module 2 (Tasks 3 & 7):** Edge Detection (Sobel, Roberts, Prewitt, Canny) & Entropy Calculation.
* **Module 3 (Tasks 4 & 8):** Histograms/Distribution Curves & Color Transformations.
* **Module 4 (Tasks 5 & 6):** Image Equalization & Normalization.
* **Module 5 (Tasks 9 & 10):** Frequency Domain Filters & Hybrid Images.

*(Team: Coordinate internally on who is taking Modules 1 through 5. Your designated workspaces are in the `src/backend/` folder.)*

---

## 🏗️ Architecture & Nesting

The project is strictly separated to prevent merge conflicts. The **Frontend** handles all Qt GUI elements, user interactions, and state management. The **Backend** is purely for mathematical OpenCV logic.

The two halves talk to each other through the `AppController`. When a user clicks "Apply", the Controller grabs the `cv::Mat` from the UI, passes it to the Backend interfaces, and pushes the returned `cv::Mat` back to the UI.

### 📂 Directory Structure & File Functions

```text
ComputerVision_Task1/
├── CMakeLists.txt                 # Build configuration and Qt/OpenCV linking
├── README.md                      # Documentation
├── .gitignore                     # Prevents pushing the build folder to GitHub
├── src/
│   ├── main.cpp                   # Application entry point (Boots Qt)
│   │
│   ├── frontend/                  # [FRONTEND DOMAIN] User Interface
│   │   ├── MainWindow.h/cpp       # Main window container; dynamically splits panels
│   │   ├── ui/MainWindow.ui       # XML layout scaffolding
│   │   │
│   │   ├── components/            # Reusable UI Widgets
│   │   │   ├── ImagePanel.h/cpp   # Holds OpenCV images. Generates its own "Load" button if it is an Input panel.
│   │   │   ├── TopTaskBar.h/cpp   # Holds the operation dropdown, Apply/Clear buttons.
│   │   │   └── ParameterBox.h/cpp # Dynamically loads sliders/dropdowns based on the selected task.
│   │   │
│   │   └── controllers/           # The Brains (Logic & State)
│   │       ├── AppController.h/cpp       # Routes data between the UI and Backend.
│   │       └── ImageStateManager.h/cpp   # Handles the Cascade vs. Clear logic.
│   │
│   └── backend/                   # [BACKEND DOMAIN] Image Processing Logic
│       ├── core/                  
│       │   ├── ImageProcessorInterface.h # The standard bridge for the AppController to call your modules.
│       │   └── Utils.h                   # Shared OpenCV helper functions.
│       │
│       ├── Module1_NoiseAndFilters/      # Team workspaces for OpenCV logic
│       ├── Module2_EdgesAndEntropy/       
│       ├── Module3_HistogramsAndColor/    
│       ├── Module4_Enhancement/           
│       └── Module5_FrequencyAndHybrid/
