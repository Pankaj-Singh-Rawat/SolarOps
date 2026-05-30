import os
import random
from flask import Flask, request, jsonify

app = Flask(__name__)

@app.route('/predict', methods=['POST'])
def predict():
    """
    Simulates real-time inference using upcoming weather forecast data vectors
    and trailing historical generation metrics.
    """
    try:
        data = request.get_json()
        if not data:
            return jsonify({"error": "Missing JSON request payload"}), 400
            
        current_yield = float(data.get("current_yield", 0.0))
        
        # Simulating external weather API variables for forecast metrics:
        # 1. Solar Irradiance Index (0.0 clear minimum to 1.0 peak exposure)
        # 2. Cloud Cover Percentage (0% clear sky to 100% full cover)
        predicted_solar_irradiance = random.uniform(0.65, 0.95) 
        predicted_cloud_cover = random.uniform(10.0, 35.0)      
        
        # Linear Model Simulation Equation:
        # Expected yield is weighed by current baseline trends and optimized 
        # by forecasted irradiance indices, with losses applied via cloud density offsets.
        base_calculation = (current_yield * 0.35) + (predicted_solar_irradiance * 16.5)
        loss_modifier = (predicted_cloud_cover / 100.0) * 4.2
        
        predicted_yield = max(0.0, base_calculation - loss_modifier)
        
        return jsonify({
            "status": "success",
            "target_date": "2026-05-31",
            "predicted_yield_kwh": round(predicted_yield, 2),
            "features_computed": {
                "forecast_irradiance_index": round(predicted_solar_irradiance, 2),
                "forecast_cloud_cover_pct": round(predicted_cloud_cover, 1)
            }
        }), 200

    except ValueError:
        return jsonify({"error": "Invalid numerical data types provided"}), 400
    except Exception as e:
        return jsonify({"error": str(e)}), 500

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=False)