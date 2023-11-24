#ifndef SPI_H
#define SPI_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Marks the beginning of a logical frame.
 * 
 */
void beginFrame(void);

/**
 * @brief Marks the end of a logical frame.
 * 
 */
void endFrame(void);

/**
 * @brief Necessary for Python models.
 * 
 */
void setupSPI(void);

#ifdef __cplusplus
}
#endif

#endif
