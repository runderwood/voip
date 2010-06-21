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
 
package gong.xml.gasi;

import java.awt.EventQueue;
import javax.swing.SwingUtilities;

/**
 * This is the script handler for the Gong scripting interface.
 * @version 1.1, 27/02/2008
 * @author Gibson Lam
 */
public abstract class ScriptHandler {

    /**
     * The delimiter between parameters
     */
    protected final static String DELIMITER = ";";

    /**
     * The Mime type of plain text
     */
    protected final static String MIME_TEXT_PLAIN = "text/plain";
    /**
     * The Mime type of HTML text
     */
    protected final static String MIME_TEXT_HTML  = "text/html";

    /**
     * Ima ADPCM WAV audio format type
     */
    protected final static String TYPE_WAV_ADPCM = "wav/adpcm";
    /**
     * Speex audio format type
     */
    protected final static String TYPE_SPEEX = "speex";
    /**
     * PCM FLV audio format type
     */
    protected final static String TYPE_FLV_PCM   = "flv/pcm";

    // XML operator
    private Request request = null;

    // Text operator
    private String name = null;
    private String[] params = null;

    // The response and fault
    private Object response = null;
    private Object fault = null;

    /**
     * Creates a new instance of ScriptHandler
     * @param request the request string
     */
    public ScriptHandler(String request) {
        // Parse the request
        Message message;
        try {
            message = Message.parse(request);
            if (message == null || !message.getClass().equals(Request.class)) throw new Exception();
            this.request = (Request) message;
        } catch (Exception e) {}
    }

    /**
     * Creates a new instance of ScriptHandler
     * @param name the request name
     * @param params parameter list of the request
     */
    public ScriptHandler(String name, String[] params) {
        this.name = name;
        this.params = params;
    }

    /**
     * Returns whether the request is an XML request
     * @return true if the request is in XML form
     */
    protected boolean isXML() {
        return (request != null);
    }

    /**
     * Sets the response object or string
     * @param object the response object or string
     */
    protected void setResponse(Object object) {
        response = object;
    }

    /**
     * Gets the response object or string
     * @return the response object or string
     */
    public Object getResponse() {
        return response;
    }

    /**
     * Sets the fault string
     * @param reason the fault reason
     */
    protected void setFault(String reason) {
        if (isXML()) {
            try {
                fault = gong.xml.gasi.Message.newFault("", reason);
            } catch (Throwable t) {}
        } else fault = reason;
    }

    /**
     * Gets the fault object
     * @return the fault object
     */
    public Object getFault() {
        return fault;
    }

    private String getRequestName() {
        if (isXML()) return request.getName();
        return (name == null)? "" : name + "Request";
    }

    /**
     * Gets the parameter in a subfield from the XML string or the parameter list
     * @param name the parameter name
     * @param position the position of the parameter in the list
     * @param subfield the subfield name
     * @return the parameter value
     * @throws java.lang.Exception failed to get the parameter
     */
    protected String getParameter(String name, int position, int subfield) throws Exception {
        if (isXML()) return request.getParameter(name);
        try {
            String param = params[position];
            if (subfield < 0) return param;
            String[] subfields = param.split(DELIMITER);
            return subfields[subfield];
        } catch (Exception ex) {
            return null;
        }
    }

    /**
     * Gets the parameter from the XML string or the parameter list
     * @param name the parameter name
     * @param position the position of the parameter in the list
     * @return the parameter value
     * @throws java.lang.Exception failed to get the parameter
     */
    protected String getParameter(String name, int position) throws Exception {
        return getParameter(name, position, -1);
    }

    /**
     * Gets the parameter in a parent field from the XML string or the parameter list
     * @param parent the name of the parent node
     * @param name the parameter name
     * @param position the position of the parameter in the list
     * @param subfield the subfield name
     * @return the parameter value
     * @throws java.lang.Exception failed to get the parameter
     */
    protected String getParameter(String parent, String name, int position, int subfield) throws Exception {
        if (isXML()) return request.getParameter(parent, name);
        return getParameter(null, position, subfield);
    }

    /**
     * Executes the request
     */
    public void execute() {
        try {
            String name = getRequestName();
            if (name.equals("SetMediaTimeRequest"))
                setMediaTime();
            else if (name.equals("GetMediaTimeRequest"))
                getMediaTime();
            else if (name.equals("GetMediaDurationRequest"))
                getMediaDuration();
            else if (name.equals("SetMediaRateRequest"))
                setMediaRate();
            else if (name.equals("GetMediaRateRequest"))
                getMediaRate();
            else if (name.equals("GetMediaStatusRequest"))
                getMediaStatus();
            else if (name.equals("GetAudioLevelRequest"))
                getAudioLevel();
            else if (name.equals("GetCurrentMessageIdRequest"))
                getCurrentMessageId();
            else if (name.equals("SearchMessageRequest"))
                searchMessage();
            else if (name.equals("GetMessageRequest"))
                getMessage();
            else if (name.equals("GetMessageContentRequest"))
                getMessageContent();
            else if (name.equals("GetCurrentTokenRequest"))
                getCurrentToken();
            else if (name.equals("GetBoardNameRequest"))
                getBoardName();
            else if (name.equals("GetBoardDataRequest"))
                getBoardData();
            else if (name.equals("GetVersionRequest"))
                getVersion();
            else {
                Operator operator = new Operator();
                if (EventQueue.isDispatchThread())
                    operator.run();
                else
                    SwingUtilities.invokeAndWait(operator);
            }
        } catch (Exception e) {
            setFault(e.getMessage());
        }
    }

    /**
     * Plays the media
     * @throws java.lang.Exception failed to play the media
     */
    protected abstract void playMedia() throws Exception;
    /**
     * Records a new media data
     * @throws java.lang.Exception failed to record the media
     */
    protected abstract void recordMedia() throws Exception;
    /**
     * Pause the current playing media
     * @throws java.lang.Exception failed to pause the media
     */
    protected abstract void pauseMedia() throws Exception;
    /**
     * Stops the current playing or recording media
     * @throws java.lang.Exception failed to stop the media
     */
    protected abstract void stopMedia() throws Exception;
    /**
     * Sets the media time
     * @throws java.lang.Exception failed to set the media time
     */
    protected abstract void setMediaTime() throws Exception;
    /**
     * Gets the media time
     * @throws java.lang.Exception failed to get the media time
     */
    protected abstract void getMediaTime() throws Exception;
    /**
     * Gets the media duration
     * @throws java.lang.Exception failed to get the media duration
     */
    protected abstract void getMediaDuration() throws Exception;
    /**
     * Sets the media playback rate
     * @throws java.lang.Exception failed to set the media rate
     */
    protected abstract void setMediaRate() throws Exception;
    /**
     * Gets the media playback rate
     * @throws java.lang.Exception failed to get the media rate
     */
    protected abstract void getMediaRate() throws Exception;
    /**
     * Gets the media status
     * @throws java.lang.Exception failed to get the media status
     */
    protected abstract void getMediaStatus() throws Exception;
    /**
     * Gets the current audio level
     * @throws java.lang.Exception failed to get the audio level
     */
    protected abstract void getAudioLevel() throws Exception;
    /**
     * Moves to the previous message
     * @throws java.lang.Exception failed to move to the previous message
     */
    protected abstract void movePrevMessage() throws Exception;
    /**
     * Moves to the next message
     * @throws java.lang.Exception failed to move to the next message
     */
    protected abstract void moveNextMessage() throws Exception;
    /**
     * Selects a message with an id
     * @throws java.lang.Exception failed to select a message
     */
    protected abstract void selectMessage() throws Exception;
    /**
     * Gets the message id of the selected message
     * @throws java.lang.Exception failed to get the current message id
     */
    protected abstract void getCurrentMessageId() throws Exception;
    /**
     * Searches for a message with a finder
     * @throws java.lang.Exception failed to search for a message
     */
    protected abstract void searchMessage() throws Exception;
    /**
     * Gets the message XML string
     * @throws java.lang.Exception failed to get the message string
     */
    protected abstract void getMessage() throws Exception;
    /**
     * Gets the message content
     * @throws java.lang.Exception failed to get the message content
     */
    protected abstract void getMessageContent() throws Exception;
    /**
     * Posts the message to the board
     * @throws java.lang.Exception failed to post the message
     */
    protected abstract void postMessage() throws Exception;
    /**
     * Saves the message to the hard disk
     * @throws java.lang.Exception failed to save the message
     */
    protected abstract void saveMessage() throws Exception;
    /**
     * Posts the current audio message to an HTML form
     * @throws java.lang.Exception failed to post the message to an HTML form
     */
    protected abstract void postToForm() throws Exception;
    /**
     * Loads a audio recording from a absolute or relative url
     * @throws java.lang.Exception failed to load a recording from an url
     */
    protected abstract void loadFromURL() throws Exception;
    /**
     * Gets the current playing token in the message text
     * @throws java.lang.Exception failed to get the current playing token
     */
    protected abstract void getCurrentToken() throws Exception;
    /**
     * Gets the current board name
     * @throws java.lang.Exception failed to get the board name
     */
    protected abstract void getBoardName() throws Exception;
    /**
     * Gets the current board data
     * @throws java.lang.Exception failed to get the board data
     */
    protected abstract void getBoardData() throws Exception;
    /**
     * Gets the version string
     * @throws java.lang.Exception failed to the version string
     */
    protected abstract void getVersion() throws Exception;
    
    private class Operator extends Thread {

        public void run() {
            try {
                String name = getRequestName();
                if (name.equals("PlayMediaRequest"))
                    playMedia();
                else if (name.equals("RecordMediaRequest"))
                    recordMedia();
                else if (name.equals("PauseMediaRequest"))
                    pauseMedia();
                else if (name.equals("StopMediaRequest"))
                    stopMedia();
                else if (name.equals("MoveToPrevMessageRequest"))
                    movePrevMessage();
                else if (name.equals("MoveToNextMessageRequest"))
                    moveNextMessage();
                else if (name.equals("SelectMessageRequest"))
                    selectMessage();
                else if (name.equals("PostMessageRequest"))
                    postMessage();
                else if (name.equals("SaveMessageRequest"))
                    saveMessage();
                else if (name.equals("PostToFormRequest"))
                    postToForm();
                else if (name.equals("LoadFromURLRequest"))
                    loadFromURL();
                else throw new Exception("You have made an invalid request.");
            } catch (Exception e) {
                setFault(e.getMessage());
            }
        }

    }

}
