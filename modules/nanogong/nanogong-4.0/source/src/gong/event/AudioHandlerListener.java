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
 
package gong.event;

import gong.audio.AudioHandler;

/**
 * This interface defines a listener for the audio handler.
 * @author Gibson Lam
 * @version 1.0, 18/03/2005
 */
public interface AudioHandlerListener {

    /**
     * Receives the updated time from the audio handler
     * @param handler the audio handler
     * @param time the updated time
     */
    public void timeUpdate(AudioHandler handler, long time);
    
    /**
     * Receives the updated status from the audio handler
     * @param handler the audio handler
     * @param status the updated status
     */
    public void statusUpdate(AudioHandler handler, int status);

    /**
     * Receives the updated duration from the audio handler
     * @param handler the audio handler
     * @param duration the updated duration
     */
    public void durationUpdate(AudioHandler handler, long duration);
    
    /**
     * Receives the updated amplitude level of the audio handler
     * @param handler the audio handler
     * @param amplitude the updated amplitude level
     */
    public void amplitudeUpdate(AudioHandler handler, float amplitude);

}
