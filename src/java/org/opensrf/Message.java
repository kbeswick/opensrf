package org.opensrf;
import org.opensrf.util.*;


public class Message implements OSRFSerializable {

    /** Message types */
    public static final String REQUEST = "REQUEST";
    public static final String STATUS = "STATUS";
    public static final String RESULT = "RESULT";
    public static final String CONNECT = "CONNECT";
    public static final String DISCONNECT = "DISCONNECT";

    /** Message ID.  This number is used to relate requests to responses */
    private int id;
    /** type of message. */
    private String type;
    /** message payload */
    private Object payload;
    /** message locale */
    private String locale;

    /** Create a registry for the osrfMessage object */
    private static OSRFRegistry registry = 
        OSRFRegistry.registerObject(
            "osrfMessage", 
            OSRFRegistry.WireProtocol.HASH, 
            new String[] {"threadTrace", "type", "payload", "locale"});

    /**
     * @param id This message's ID
     * @param type The type of message
     */
    public Message(int id, String type) {
        setId(id);
        setString(type);
    }

    /**
     * @param id This message's ID
     * @param type The type of message
     * @param payload The message payload
     */
    public Message(int id, String type, Object payload) {
        this(id, type);
        setPayload(payload);
    }

    /**
     * @param id This message's ID
     * @param type The type of message
     * @param payload The message payload
     * @param locale The message locale
     */
    public Message(int id, String type, Object payload, String locale) {
        this(id, type, payload);
        setPayload(payload);
        setLocale(locale);
    }


    public int getId() {
        return id;
    }   
    public String getType() {
        return type;
    }
    public Object getPayload() {
        return payload;
    }
    public String getLocale() {
        return locale;
    }
    public void setId(int id) {
        this.id = id;
    }
    public void setString(String type) {
        this.type = type;
    }
    public void setPayload(Object p) {
        payload = p;
    }
    public void setLocale(String l) {
        locale = l;
    }

    /**
     * Implements the generic get() API required by OSRFSerializable
     */
    public Object get(String field) {
        if("threadTrace".equals(field))
            return getId();
        if("type".equals(field))
            return getType().toString();
        if("payload".equals(field))
            return getPayload();
        if("locale".equals(field))
            return getLocale();
        return null;
    }

    /**
     * @return The osrfMessage registry.
     */
    public OSRFRegistry getRegistry() {
        return registry;
    }
}


