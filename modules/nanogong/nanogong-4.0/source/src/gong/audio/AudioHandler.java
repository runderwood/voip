/*
 * Copyright 2002-2008 The Gong Project (http://gong.ust.hk)
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
package gong.audio;

import gong.Utility;
import java.net.URL;
import java.net.URLConnection;
import javax.sound.sampled.DataLine.Info;
import javax.sound.sampled.FloatControl.Type;
import gong.event.AudioHandlerListener;
import java.awt.Toolkit;
import java.io.BufferedInputStream;
import java.net.ConnectException;
import gong.audio.data.ImaADPCMData;
import java.io.File;
import java.util.Enumeration;
import java.util.Vector;
import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.FloatControl;
import javax.sound.sampled.SourceDataLine;
import javax.sound.sampled.TargetDataLine;

/**
 * Class AudioHandler contains all audio related APIs that can be used by other
 * classes.
 * @author Gibson Lam
 * @version 3.1, 20/08/2008
 * @version 4.0, 27/04/2010
 */
public class AudioHandler {
    
    /**
     * The capturing format in java sound
     */
    public final static AudioFormat CAPTURE_FORMAT = new AudioFormat(44100, 16, 1, true, true);
    
    /**
     * The data format in java sound
     */
    public final static AudioFormat DATA_FORMAT = new AudioFormat(11025, 16, 1, true, true);

    /**
     * The data format used to store the sound
     */
    protected AudioFormat dataFormat = null;

    /**
     * The data used to record the sound
     */
    protected AudioData recordData = null;
    
    /**
     * The playback format in java sound
     */
    public final static AudioFormat PLAYBACK_FORMAT = new AudioFormat(44100, 16, 1, true, true);
    
    /**
     * The maximum duration that can be recorded
     */
    public final static long MAX_DURATION = 300000;
    
    /**
     * A constant that represents the recording state.
     */
    public final static int RECORDING = 1;
    
    /**
     * A constant that represents the playing state.
     */
    public final static int PLAYING = 2;
    
    /**
     * A constant that represents the sending state.
     */
    public final static int SENDING = 3;
    
    /**
     * A constant that represents the playing paused state.
     */
    public final static int PAUSED = 4;
    
    /**
     * A constant that represents the recording paused state.
     */
    public final static int PAUSED_RECORD = 5;
    
    /**
     * A constant that represents the stopping state.
     */
    public final static int STOPPING = 6;
    
    /**
     * A constant that represents the stopped state.
     */
    public final static int STOPPED = 7;
    
    /**
     * A constant that represents the closing state.
     */
    public final static int CLOSING = 8;
    
    /**
     * A constant that represents the closed state.
     */
    public final static int CLOSED = 9;
    
    /**
     * A constant that represents the receiving state.
     */
    public final static int RECEIVING = 10;
    
    /**
     * The audio data storage
     */
    protected AudioData audioData = null;
    
    /**
     * The url pointing to a media file
     */
    protected String url = null;
    
    /**
     * The current media time of the audio handler
     */
    protected long time = 0;
    
    /**
     * The duration of the current media
     */
    protected long duration = 0;
    
    /**
     * The status of the media handler
     */
    protected int status = CLOSED;
    
    /**
     * True if the data is silence
     */
    protected boolean silenceData = false;
    
    /**
     * The listeners of the handler
     */
    protected Vector listeners = new Vector();
    
    /**
     * The audio recorder
     */
    protected Recorder recorder = null;
    
    /**
     * The audio player
     */
    protected Player player = null;
    
    /**
     * The playback rate
     */
    protected float rate = 1.0f;
    
    /**
     * True if the data is buffering at the audio player
     */
    protected boolean buffering = false;
    
    /**
     * The current amplitude of the handler
     */
    protected float amplitude = 0;
    
    /**
     * Adds a listener
     * @param listener the audio handler listener
     */
    public void addListener(AudioHandlerListener listener) {
        listeners.add(listener);
        
        if (listener != null) {
            listener.statusUpdate(this, status);
            listener.timeUpdate(this, time);
            listener.durationUpdate(this, duration);
            listener.amplitudeUpdate(this, 0);
        }
    }
    
    /**
     * Removes a listener
     * @param listener the audio handler listener
     */
    public void removeListener(AudioHandlerListener listener) {
        listeners.remove(listener);
    }
    
    /**
     * Sets listeners
     * @param listeners the audio handler listeners
     */
    public void setListeners(Vector listeners) {
        this.listeners = (Vector) listeners.clone();
        
        if (listeners.size() > 0) {
            for (Enumeration e = listeners.elements(); e.hasMoreElements();) {
                AudioHandlerListener listener = (AudioHandlerListener) e.nextElement();
                listener.statusUpdate(this, status);
                listener.timeUpdate(this, time);
                listener.durationUpdate(this, duration);
                listener.amplitudeUpdate(this, 0);
            }
        }
    }
    
    /**
     * Clear all listeners
     */
    public void clearListeners() {
        listeners.clear();
    }
    
    /**
     * Gets the listeners
     * @return the audio handler listeners
     */
    public Vector getListeners() {
        return listeners;
    }
    
    /**
     * Sets the duration of the handler
     * @param duration the duration
     */
    public void setDuration(long duration) {
        this.duration = duration;
        if (listeners.size() > 0) {
            for (Enumeration e = listeners.elements(); e.hasMoreElements();) {
                AudioHandlerListener listener = (AudioHandlerListener) e.nextElement();
                listener.durationUpdate(this, duration);
            }
        }
    }
    
    /**
     * Returns the duration of the handler.
     * @return the duration
     */
    public long getDuration() {
        return duration;
    }
    
    /**
     * Sets the status of the handler
     * @param status the status
     */
    public void setStatus(int status) {
        if (this.status != status) {
            this.status = status;
            if (listeners.size() > 0) {
                for (Enumeration e = listeners.elements(); e.hasMoreElements();) {
                    AudioHandlerListener listener = (AudioHandlerListener) e.nextElement();
                    listener.statusUpdate(this, status);
                }
            }
        }
    }
    
    /**
     * Returns the status of the handler
     * @return the status
     */
    public int getStatus() {
        return status;
    }
    
    /**
     * Sets the data of the handler
     * @param audioData the audio data object
     */
    public void setData(AudioData audioData) {
        this.audioData = audioData;
        if (audioData != null) setDuration(audioData.getDuration());
        if (audioData != null || url != null)
            setStatus(STOPPED);
        else
            setStatus(CLOSED);
    }
    
    /**
     * Returns the data of the handler
     * @return the audio data
     */
    public AudioData getData() {
        return audioData;
    }
    
    /**
     * Sets the url of the handler
     * @param url the url
     */
    public void setURL(String url) {
        if (url != null && url.length() == 0)
            this.url = null;
        else
            this.url = url;
        
        if (audioData != null || url != null)
            setStatus(STOPPED);
        else
            setStatus(CLOSED);
    };
    
    /**
     * Returns the url of the handler
     * @return the url
     */
    public String getURL() {
        return url;
    }
    
    /**
     * Sets the rate of the playback
     * @param rate the rate
     */
    public void setRate(float rate) {
        this.rate = rate;
        if (player != null) player.setRate(rate);
    }
    
    /**
     * Gets the rate of the playback
     * @return the rate
     */
    public float getRate() {
        return rate;
    }
    
    /**
     * Return whether the handler has audio data
     * @return true if data exists
     */
    public boolean hasData() {
        if (audioData != null || (url != null && url.length() > 0)) return true;
        return false;
    }
    
    /**
     * Downloads the data from the url
     * @param cache the cached file location
     * @param synchronous true if the download is synchronized (blocking)
     * @throws java.lang.Exception failed to download the data
     */
    public void downloadData(File cache, boolean synchronous) throws Exception {
        if (audioData == null && !hasData()) throw new Exception("Failed to download empty URL.");
        
        if (audioData != null) {
            while (audioData.isTransferInProgress()) {
                try {
                    Thread.currentThread().sleep(100);
                } catch (Throwable t) {}
            }
        } else {
            BufferedInputStream stream;
            try {
                URL url = new URL(Utility.encodeURL(getURL()));
                URLConnection connection = url.openConnection();
                connection.setUseCaches(false);
                
                stream = new BufferedInputStream(connection.getInputStream());
            } catch (Exception e) {
                System.out.println(e.getMessage());
                throw new Exception("Failed to establish connection to the server.");
            }

            try {
                AudioData audioData = AudioData.createFromStream(stream);
                audioData.setCache(cache);
                audioData.receiveFromStream(stream, synchronous);

                setURL(null);
                setData(audioData);
            } catch (ConnectException e) {
                throw new Exception("Failed to transfer voice data.");
            } catch (Exception e) {
                throw new Exception("Failed to open incompatible/unavailable voice data.");
            }
        }
    }
    
    /**
     * Sets the current media time
     * @param time the time in milliseconds
     */
    public void setTime(long time) {
        this.time = time;
        
        if (player != null) {
            player.setTime(time);
        } else if (listeners.size() > 0) {
            for (Enumeration e = listeners.elements(); e.hasMoreElements();) {
                AudioHandlerListener listener = (AudioHandlerListener) e.nextElement();
                listener.timeUpdate(this, time);
            }
        }
    }
    
    /**
     * Gets the current media time
     * @return the media time in milliseconds
     */
    public long getTime() {
        return time;
    }
    
    /**
     * Sets the playing volume of the handler
     * @param level the volume level between 0 to 1
     */
    public void setVolume(float level) {
        if (player != null) player.setVolume(level);
    }
    
    /**
     * Gets the buffering status of the player
     * @return true if the player is buffering data
     */
    public boolean getBuffering() {
        return buffering;
    }
    
    /**
     * Gets the amplitude of the handler
     * @return the amplitude level
     */
    public float getAmplitude() {
        return amplitude;
    }
    
    /**
     * This class contains the audio recorder.
     */
    protected class Recorder extends Thread {
        
        /** The minimum level of silent sample */
        private final static float MIN_SILENCE_LEVEL = 0.01f;
        
        /** The minimum percentage of samples for silence data */
        private final static float MIN_SILENCE_PERCENTAGE = 0.9f;
        
        private AudioHandler handler;
        private TargetDataLine line;
        private AudioFormat sourceFormat;
        private AudioFormat targetFormat;
        private boolean paused = false;
        private boolean stopped = false;
        private float maxAmplitude = 0;
        private int amplSampleCount = 0;
        private float sampleCount = 0;
        private long totalBufferBlock = 0;
        private long silenceBufferBlock = 0;
        private long timeToStop = 0;
        
        /**
         * Creates a new instance of Recorder
         */
        public Recorder(AudioHandler handler, AudioFormat sourceFormat, AudioFormat targetFormat, long timeToStop) {
            this.handler = handler;
            this.sourceFormat = sourceFormat;
            this.targetFormat = targetFormat;
            this.timeToStop = timeToStop;
        }
        
        public void open() throws Exception {
            // Prepare the audio system
            Info info = new Info(TargetDataLine.class, sourceFormat);
            if (!AudioSystem.isLineSupported(info)) throw new Exception("Failed to initialize audio recorder.");
            
            line = (TargetDataLine) AudioSystem.getLine(info);
            line.open(sourceFormat, line.getBufferSize());
            
            // Empty the audio buffer
            setData(null);
        }
        
        public void pause() {
            paused = true;
        }
        
        public boolean isPaused() {
            return paused;
        }
        
        public void resumeRecord() {
            paused = false;
        }
        
        public void kill() {
            stopped = true;
            while (getDuration() != audioData.getDuration()) {
                try {
                    sleep(100);
                } catch (Exception e) {}
            }
        }
        
        public void run() {
            int bufferLengthInFrames = line.getBufferSize() / 8;
            int bufferLengthInBytes = bufferLengthInFrames * sourceFormat.getFrameSize();
            byte[] buffer = new byte[bufferLengthInBytes];
            
            int numBytesRead;
            long time = 0;
            
            line.start();
            
            // Create a new audio data
            if (recordData == null)
                audioData = new ImaADPCMData(targetFormat);
            else {
                audioData = recordData;
                recordData = null;
            }
            
            float step = sourceFormat.getSampleRate() / targetFormat.getSampleRate();
            
            while (!stopped) {
                numBytesRead = line.read(buffer, 0, bufferLengthInBytes);
                if (numBytesRead == -1) break;
                
                // If the recorder is paused
                if (paused) continue;

                int silence = 0;
                for (int index = 0; index < numBytesRead; index+= 2) {
                    int sample = ((buffer[index] << 8) | (buffer[index + 1] & 0xFF));
                    
                    // Get the sample amplitude ratio and silence level
                    float ratio = (float) sample / 32768f;
                    if (ratio < 0) ratio = -ratio;
                    if (ratio < MIN_SILENCE_LEVEL) silence++;
                    
                    if (sampleCount < 1) {
                        try {
                            audioData.write(sample);
                        } catch (Throwable t) {}
                        
                        // Get the max amplitude value
                        if (ratio > maxAmplitude) maxAmplitude = ratio;
                        
                        // Update display for every fifth of a second
                        amplSampleCount++;
                        if (amplSampleCount == (int) targetFormat.getSampleRate() / 5) {
                            amplitude = maxAmplitude;
                            if (listeners.size() > 0) {
                                for (Enumeration e = listeners.elements(); e.hasMoreElements();) {
                                    AudioHandlerListener listener = (AudioHandlerListener) e.nextElement();
                                    listener.amplitudeUpdate(handler, maxAmplitude);
                                }
                            }
                            
                            amplSampleCount = 0;
                            maxAmplitude = 0;
                        }
                    }
                    sampleCount++;
                    if (sampleCount >= step) sampleCount -= step;
                }
                
                if ((float) silence / (float) (numBytesRead / 2f) > MIN_SILENCE_PERCENTAGE)
                    silenceBufferBlock++;
                totalBufferBlock++;
                
                // Update the time
                time = audioData.getTime();
                if (listeners.size() > 0) {
                    for (Enumeration e = listeners.elements(); e.hasMoreElements();) {
                        AudioHandlerListener listener = (AudioHandlerListener) e.nextElement();
                        listener.timeUpdate(handler, time);
                    }
                }
                if (time > timeToStop) {
                    Toolkit.getDefaultToolkit().beep();
                    break;
                }
            }
            
            line.stop();
            line.close();

            try {
                audioData.close();
            }
            catch (Exception e) {}
            
            time = 0;
            amplitude = 0;
            if (listeners.size() > 0) {
                for (Enumeration e = listeners.elements(); e.hasMoreElements();) {
                    AudioHandlerListener listener = (AudioHandlerListener) e.nextElement();
                    listener.timeUpdate(handler, time);
                    listener.amplitudeUpdate(handler, 0);
                }
            }
            
            setDuration(audioData.getDuration());
            
            if (!stopped) notifyStop();

        }
        
        /** Determine whether the capture is silence
         * @return true if silence; false otherwise
         */
        public boolean isSilence() {
            if ((float) silenceBufferBlock / (float) totalBufferBlock > MIN_SILENCE_PERCENTAGE)
                return true;
            return false;
        }
        
    }

    /**
     * Sets the data format used in recording the sound
     * @param format the data format
     */
    public void setDataFormat(AudioFormat format) {
        dataFormat = format;
    }

    /**
     * Sets the recording audio data format
     * @param data the audio data
     */
    public void setRecordData(AudioData data) {
        recordData = data;
    }
    
    /**
     * Starts recording with the handler
     * @param timeToStop the time to stop the recording
     * @throws java.lang.Exception failed to start recording
     */
    public synchronized void record(long timeToStop) throws Exception {
        if (recorder == null) {
            // Start a new recorder
            try {
                if (dataFormat == null) dataFormat = DATA_FORMAT;
                recorder = new Recorder(this, CAPTURE_FORMAT, dataFormat, timeToStop);
                recorder.open();
                recorder.start();
            } catch (Exception e) {
                recorder = null;
                throw new Exception("Failed to initialize audio recorder.");
            }
        }
        else if (recorder.isPaused()) {
            // Resume the recording ** timeToStop is ignored
            recorder.resumeRecord();
        }
            
        url = null;
        
        setStatus(RECORDING);
    }
    
    /**
     * This class contains the audio player.
     */
    protected class Player extends Thread {
        
        private AudioHandler handler;
        private SourceDataLine line;
        private AudioFormat sourceFormat;
        private AudioFormat targetFormat;
        private boolean stopped = false;
        private long timeToStart;
        private long timeToStop;
        private int delay = 0;
        private float rate = 1.0f;
        private OlaBuffer olaBuffer;
        private byte[] buffer;
        private int position;
        private float maxAmplitude = 0;
        
        /**
         * Creates a new instance of Player
         * @param handler the audio handler
         * @param sourceFormat the source audio format
         * @param targetFormat the playback target format
         * @param timeToStop the time to stop the play back
         */
        public Player(AudioHandler handler, AudioFormat sourceFormat, AudioFormat targetFormat, long timeToStop) {
            this.handler = handler;
            this.sourceFormat = sourceFormat;
            this.targetFormat = targetFormat;
            this.timeToStop = timeToStop;

            olaBuffer = new OlaBuffer(sourceFormat, rate);
        }
        
        /**
         * Opens the audio player
         * @throws java.lang.Exception failed to open the player
         */
        public void open() throws Exception {
            Info info = new Info(SourceDataLine.class, targetFormat);
            if (!AudioSystem.isLineSupported(info)) {
                throw new Exception("Line matching " + info + " not supported.");
            }
            
            line = (SourceDataLine) AudioSystem.getLine(info);
            line.open(targetFormat, (int) (targetFormat.getFrameRate() * targetFormat.getFrameSize() / 4f));
            delay = (int) ((float) line.getBufferSize() / (targetFormat.getFrameRate() * targetFormat.getFrameSize()) * 1000f) + 15;
            
            setTime(0);
            
            audioData.reset();

            buffer = new byte[(int) (targetFormat.getFrameRate() * targetFormat.getFrameSize() / 4f)];
            position = 0;
        }
        
        /**
         * Stops the audio player
         */
        public void kill() {
            stopped = true;
        }
        
        private void write(int sample) {
            buffer[2 * position] = (byte) ((sample >> 8) & 0xFF);
            buffer[2 * position + 1] = (byte) (sample & 0xFF);
            position++;
            if (position >= buffer.length / 2) {
                line.write(buffer, 0, 2 * position);
                position = 0;
            }

            // Get the sample amplitude ratio
            float ratio = (float) sample / 32768f;
            if (ratio < 0) ratio = -ratio;
            
            // Get the max amplitude value
            if (ratio > maxAmplitude) maxAmplitude = ratio;
        }
        
        /**
         * Starts the audio player
         */
        public void run() {
            line.start();
            
            int updateInterval = (int) sourceFormat.getSampleRate() / 5;
            int updateSampleCount = 0;
            float sampleCount = 0;
            float step = targetFormat.getSampleRate() / sourceFormat.getSampleRate();
            
            int sample;
            long time = 0, duration = audioData.getDuration();
            
            while (!stopped) {
                // Buffer the data
                if (audioData.getTime() == 0 || !audioData.isAvailable()) {
                    buffering = true;
                    while (!audioData.isTransferBuffered(rate)) {
                        try {
                            sleep(100);
                        } catch (Exception e) {}
                        if (stopped) break;
                    }
                    buffering = false;
                }
                if (stopped) break;
                
                try {
                    synchronized (this) {
                        if (audioData.isAvailable()) {
                            sample = audioData.read();

                            while (sampleCount < step) {
                                olaBuffer.write(sample);
                                sampleCount++;

                                while (olaBuffer.isAvailable()) write(olaBuffer.read());
                            }
                            sampleCount -= step;
                        
                            // Update the time
                            time = audioData.getTime();
                            if ((++updateSampleCount) % updateInterval == 0) {
                                if (timeToStop <= 0 || time <= timeToStop) {
                                    if (time - delay > timeToStart) {
                                        listenerUpdate(time - delay, maxAmplitude);
                                    }
                                    if (timeToStop > 0) {
                                        if (timeToStop - time < delay) new Timer(delay, time, maxAmplitude).start();
                                    } else {
                                        if (duration - time < delay) new Timer(delay, time, maxAmplitude).start();
                                    }
                                    maxAmplitude = 0;
                                }
                            }
                        }
                    }
                
                    // Time to stop
                    if (timeToStop > 0 && time > timeToStop) break;

                    // The end of stream
                    if (time == duration) break;
                } catch (Exception e) {
                    break;
                }
            }
            
            olaBuffer.drain();
            while (olaBuffer.isAvailable()) write(olaBuffer.read());
            if (position > 0) line.write(buffer, 0, 2 * position);
            
            if (stopped)
                line.flush();
            else
                line.drain();
            
            line.stop();
            line.close();
            
            if (!stopped) notifyStop();
        }
        
        /**
         * Sets the volume of the player
         * @param level the amplitude level
         */
        public void setVolume(float level) {
            FloatControl gain = (FloatControl) line.getControl(Type.MASTER_GAIN);
            if (gain != null) {
                if (level == 0f) level = 0.0001f;
                float dB = (float) (Math.log(level) / Math.log(10f) * 20f);
                gain.setValue(dB);
            }
        }
        
        /**
         * Sets the media time of the player
         * @param time the media time
         */
        public void setTime(long time) {
            try {
                synchronized (this) {
                    audioData.setTime(time);
                    AudioHandler.this.time = time;
                    timeToStart = time;
                }
            } catch (Throwable t) {}
        }
        
        /**
         * Sets the playback rate of the player
         * @param rate the playback rate
         */
        public void setRate(float rate) {
            try {
                synchronized (this) {
                    this.rate = rate;
                    if (olaBuffer != null) olaBuffer.setRate(rate);
                }
            } catch (Throwable t) {}
        }
        
        private void listenerUpdate(long time, float amplitude) {
            if (!stopped) {
                AudioHandler.this.time = time;
                for (Enumeration e = listeners.elements(); e.hasMoreElements();) {
                    AudioHandlerListener listener = (AudioHandlerListener) e.nextElement();
                    listener.timeUpdate(handler, time);
                    listener.amplitudeUpdate(handler, amplitude);
                }
            }
        }
        
        private class Timer extends Thread {
            
            private int delay;
            private long time;
            private float amplitude;
            
            public Timer(int delay, long time, float amplitude) {
                this.delay = delay;
                this.time = time;
                this.amplitude = amplitude;
            }
            
            public void run() {
                try {
                    sleep(delay);
                } catch (Throwable t) {}
                synchronized (this) {
                    listenerUpdate(time, amplitude);
                }
            }
            
        }
    }
    
    /**
     * Starts to play the media
     * @param startTime the start time to be played
     * @param timeToStop the stop time of the audio player
     * @throws java.lang.Exception failed to play the media
     */
    public synchronized void play(long startTime, long timeToStop) throws Exception {
        if (player == null) {
            try {
                player = new Player(this, audioData.getFormat(), PLAYBACK_FORMAT, timeToStop);
        
                player.open();
                player.setRate(rate);
                
                setTime(startTime);
                
                player.start();
            } catch (Exception e) {
                player = null;
                throw new Exception("Failed to initialize audio player.");
            }
        }
        
        setStatus(PLAYING);
    }
    
    /**
     * Starts to play the media
     * @param startTime the start time to be played
     * @throws java.lang.Exception failed to play the media
     */
    public synchronized void play(long startTime) throws Exception {
        play(startTime, 0);
    }
    
    /**
     * Pauses the playing/recording media
     */
    public synchronized void pause() {
        // Pause the player
        if (player != null) {
            player.kill();
            player = null;
        
            setStatus(PAUSED);
            
            if (listeners.size() > 0) {
                for (Enumeration e = listeners.elements(); e.hasMoreElements();) {
                    AudioHandlerListener listener = (AudioHandlerListener) e.nextElement();
                    listener.timeUpdate(this, time);
                    listener.durationUpdate(this, duration);
                    listener.amplitudeUpdate(this, 0);
                }
            }
        }

        // Pause the recorder
        if (recorder != null) {
            recorder.pause();
        
            setStatus(PAUSED_RECORD);
            
            if (listeners.size() > 0) {
                for (Enumeration e = listeners.elements(); e.hasMoreElements();) {
                    AudioHandlerListener listener = (AudioHandlerListener) e.nextElement();
                    listener.amplitudeUpdate(this, 0);
                }
            }
        }
    }
    
    /**
     * Tells the audio handler the player has stopped
     */
    public void notifyStop() {
        try {
            stop();
        } catch (Exception e) {}
    }
    
    /**
     * Stops the audio handler
     * @throws java.lang.Exception failed to stop the audio handler
     */
    public synchronized void stop() throws Exception {
        if (status == STOPPED) return;
        
        setStatus(STOPPING);
        
        if (recorder != null) {
            recorder.kill();
            silenceData = recorder.isSilence();
            recorder = null;
        }
        
        if (player != null) {
            player.kill();
            player = null;
        }
        
        setTime(0);
        if (audioData != null) audioData.reset();
        
        setStatus(STOPPED);
        
        if (listeners.size() > 0) {
            for (Enumeration e = listeners.elements(); e.hasMoreElements();) {
                AudioHandlerListener listener = (AudioHandlerListener) e.nextElement();
                listener.timeUpdate(this, time);
                listener.durationUpdate(this, duration);
                listener.amplitudeUpdate(this, 0);
            }
        }
    }
    
    /**
     * Closes the audio handler
     * @throws java.lang.Exception failed to close the audio handler
     */
    public synchronized void close() throws Exception {
        if (status == CLOSED) return;
        
        setStatus(CLOSING);
        
        if (recorder != null) {
            recorder.kill();
            recorder = null;
        }
        
        if (player != null) {
            player.kill();
            player = null;
        }
        
        setStatus(CLOSED);
        
        if (listeners.size() > 0) {
            for (Enumeration e = listeners.elements(); e.hasMoreElements();) {
                AudioHandlerListener listener = (AudioHandlerListener) e.nextElement();
                listener.timeUpdate(this, time);
                listener.durationUpdate(this, duration);
                listener.amplitudeUpdate(this, 0);
            }
        }
    }
    
    /**
     * Determines whether the data is silence
     * @return true if the data is silence
     */
    public boolean isSilence() {
        return silenceData;
    }
    
    /**
     * Sets whether the data is silence
     * @param silence true if the data is silence
     */
    public void setSilence(boolean silence) {
        silenceData = silence;
    }
    
}
