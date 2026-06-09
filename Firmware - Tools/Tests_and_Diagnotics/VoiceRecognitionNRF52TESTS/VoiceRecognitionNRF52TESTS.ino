#include <PDM.h>
#include <PylouVoiceRecognitionReaver_inferencing.h>

// If your target is limited in memory remove this macro to save 10K RAM
#define EIDSP_QUANTIZE_FILTERBANK   0

/** Audio buffers, pointers and selectors */
typedef struct {
    int16_t *buffer;
    uint8_t buf_ready;
    uint32_t buf_count;
    uint32_t n_samples;
} inference_t;

static inference_t inference;
static signed short sampleBuffer[2048];
static bool debug_nn = false; 

int VRC; //choose action on VoiceRecognition

float confidence = 0.8;

int VoiceRecogntion(){
  if (inference.buf_ready == 0) {
        return -1; 
  }
  signal_t signal;
  signal.total_length = EI_CLASSIFIER_RAW_SAMPLE_COUNT;
  signal.get_data = &microphone_audio_signal_get_data;
  ei_impulse_result_t result = { 0 };

  EI_IMPULSE_ERROR r = run_classifier_continuous(&signal, &result, debug_nn);
  if (r != EI_IMPULSE_OK) {
      ei_printf("ERR: Failed to run classifier (%d)\n", r);
      return -1;
  }

  // Analyse the predictions
  ei_printf("Predictions ");
  ei_printf("(DSP: %d ms., Classification: %d ms., Anomaly: %d ms.)",
  result.timing.dsp, result.timing.classification, result.timing.anomaly);
  for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
      if(result.classification[ix].value>confidence){
        /* //compare label to these names to return int since return string is a bit complicated
        if (strcmp(result.classification[ix].label, "reaver") == 0) return 0;
        else if (strcmp(result.classification[ix].label, "allumeToi") == 0) return 1;
        else if (strcmp(result.classification[ix].label, "fumee") == 0) return 2;
        else{return 3;}; */ //noise = 2 allumetoi = 0 reaver = 3 fumee = 1
        ei_printf("    %s: %.5f %d\n", result.classification[ix].label, result.classification[ix].value, ix);
      } 
      
  }
  #if EI_CLASSIFIER_HAS_ANOMALY == 1
      ei_printf("    anomaly score: %.3f\n", result.anomaly);
  #endif
  /* int total_samples = EI_CLASSIFIER_RAW_SAMPLE_COUNT; 
  
  // Définit la taille du "pas" de glissement (ici : un quart de seconde)
  int slice_samples = total_samples / 4;              

  // On décale les 3/4 récents du son vers le début de la mémoire
  memmove(inference.buffer, 
          inference.buffer + slice_samples, 
          (total_samples - slice_samples) * sizeof(int16_t));

  // On indique au compteur d'enregistrement qu'il est déjà rempli aux 3/4
  inference.buf_count = total_samples - slice_samples;

  // On autorise le microphone à enregistrer UNIQUEMENT le 1/4 restant ! */
  inference.buf_ready = 0;
  return -1;
}

static void pdm_data_ready_inference_callback(void)
{
    int bytesAvailable = PDM.available();

    // read into the sample buffer
    int bytesRead = PDM.read((char *)&sampleBuffer[0], bytesAvailable);

    if (inference.buf_ready == 0) {
        for(int i = 0; i < bytesRead>>1; i++) {
            inference.buffer[inference.buf_count++] = sampleBuffer[i];

            if(inference.buf_count >= inference.n_samples) {
                inference.buf_count = 0;
                inference.buf_ready = 1;
                break;
            }
        }
    }
}

static bool microphone_inference_start(uint32_t n_samples)
{
    inference.buffer = (int16_t *)malloc(n_samples * sizeof(int16_t));

    if(inference.buffer == NULL) {
        return false;
    }

    inference.buf_count  = 0;
    inference.n_samples  = n_samples;
    inference.buf_ready  = 0;

    // configure the data receive callback
    PDM.onReceive(&pdm_data_ready_inference_callback);

    PDM.setBufferSize(4096);

    // initialize PDM with:
    // - one channel (mono mode)
    // - a 16 kHz sample rate
    if (!PDM.begin(1, EI_CLASSIFIER_FREQUENCY)) {
        ei_printf("Failed to start PDM!");
        microphone_inference_end();

        return false;
    }

    // set the gain, defaults to 20
    PDM.setGain(30);

    return true;
}

/**
 * @brief      Wait on new data
 *
 * @return     True when finished
 */
/* static bool microphone_inference_record(void)
{
    inference.buf_ready = 0;
    inference.buf_count = 0;

    while(inference.buf_ready == 0) {
        delay(10);
    }

    return true;
} */

/**
 * Get raw audio signal data
 */
static int microphone_audio_signal_get_data(size_t offset, size_t length, float *out_ptr)
{
    numpy::int16_to_float(&inference.buffer[offset], out_ptr, length);

    return 0;
}

/**
 * @brief      Stop PDM and release buffers
 */
static void microphone_inference_end(void)
{
    PDM.end();
    free(inference.buffer);
}


void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Edge Impulse Inferencing CONTINUOUS");

  // summary of inferencing settings (from model_metadata.h)
  ei_printf("Inferencing settings:\n");
  ei_printf("\tInterval: %.2f ms.\n", (float)EI_CLASSIFIER_INTERVAL_MS);
  ei_printf("\tFrame size: %d\n", EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE);
  ei_printf("\tSample length: %d ms.\n", EI_CLASSIFIER_RAW_SAMPLE_COUNT / 16);
  ei_printf("\tNo. of classes: %d\n", sizeof(ei_classifier_inferencing_categories) / sizeof(ei_classifier_inferencing_categories[0]));

  if (microphone_inference_start(EI_CLASSIFIER_RAW_SAMPLE_COUNT) == false) {
      ei_printf("ERR: Could not allocate audio buffer (size %d), this could be due to the window length of your model\r\n", EI_CLASSIFIER_RAW_SAMPLE_COUNT);
      return;
  }

}

void loop() {
  int VRC = VoiceRecogntion();
  switch(VRC){
    case -1: break; //nothing recognised
    case 0: Serial.println("number0"); break;//do smthg
    case 1: Serial.println("number1"); break; //do smthg
    case 2: Serial.println("number2"); break; //do smthg
  }
  Serial.println("not blocked!");
  delay(100);

}

