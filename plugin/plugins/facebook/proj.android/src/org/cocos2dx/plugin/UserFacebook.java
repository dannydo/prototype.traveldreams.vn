package org.cocos2dx.plugin;

import java.util.Arrays;
import java.util.Hashtable;
import java.util.List;

import org.json.JSONException;
import org.json.JSONObject;

import com.facebook.FacebookException;
import com.facebook.FacebookOperationCanceledException;
import com.facebook.FacebookRequestError;
import com.facebook.HttpMethod;
import com.facebook.Request;
import com.facebook.RequestAsyncTask;
import com.facebook.Response;
import com.facebook.Session;
import com.facebook.Session.OpenRequest;
import com.facebook.SessionState;
import com.facebook.model.GraphObject;
import com.facebook.widget.WebDialog;
import com.facebook.widget.WebDialog.OnCompleteListener;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.provider.Settings.Secure;
import android.util.Log;
import android.view.WindowManager;

public class UserFacebook implements InterfaceUser {
	
	private static final String LOG_TAG = "UserFacebook";
	private static Activity mContext = null;
	private static UserFacebook facebook = null;
	private static boolean bDebug = false;
	
	private String appId;
	private String scope;
	private String[] permissions;
	private boolean mNeedPublishPermissions = false;
	private static SessionStatusCallback mStatusCallback = null;
	
	public boolean isRequestPermission;
	
	private static final List<String> allPublishPermissions = Arrays.asList(
			"publish_actions", "publish_stream", "ads_management",
			"create_event", "rsvp_event", "manage_friendlists",
			"manage_notifications", "manage_pages");
	
	static class Parameters {
		public static final String LINK = "link";
		public static final String PICTURE = "picture";
		public static final String NAME = "name";
		public static final String CAPTION = "caption";
		public static final String DESCRIPTION = "description";
		
		public static final String MESSAGE = "message";
		public static final String TITLE = "title";
		public static final String FILTERS = "filters";
		public static final String TO = "to";
		public static final String SUGGESTIONS = "suggestions";
		public static final String DATA = "data";
		
	}

	protected static void LogE(String msg, Exception e) {
		Log.e(LOG_TAG, msg, e);
		e.printStackTrace();
	}

	protected static void LogD(String msg) {
		if (bDebug) {
			Log.d(LOG_TAG, msg);
		}
	}

	public UserFacebook(Context context) {
		mContext = (Activity) context;
		facebook = this;
	}

	@Override
	public void configDeveloperInfo(Hashtable<String, String> cpInfo) {
		// TODO Auto-generated method stub
		LogD("initDeveloperInfo invoked " + cpInfo.toString());
		final Hashtable<String, String> curCPInfo = cpInfo;
		
		PluginWrapper.runOnMainThread(new Runnable() {
            @Override
            public void run() {
				try {
					mStatusCallback = new SessionStatusCallback();
					
					int iResourceID = mContext.getResources().getIdentifier("app_id", "string",
							mContext.getPackageName());
		    		appId = mContext.getResources().getString(iResourceID);//curCPInfo.get("FacebookAppId");
		    		scope = curCPInfo.get("FacebookScope");
		    		LogD(appId);
		    		
		    		Session session = Session.getActiveSession();
		    		if (session == null) {
		        		session = new Session.Builder(mContext).setApplicationId(appId).build();
		        		LogD("Set active session");
		    		}
	        		Session.setActiveSession(session);
		        	
		        	permissions = null;
		    		if (scope != null) {
		    			permissions = scope.split(",");
		    			for (int i = 0; i < permissions.length; i++) {
		    				if (allPublishPermissions.contains(permissions[i])) {
		    					mNeedPublishPermissions = true;
		    					break;
		    				}
		    			}
		    		}
		    	} 
		    	catch (Exception e) {
		    		LogE("Developer info is wrong!", e);
				}
            }
        });
	}

	@Override
	public void login() {
		LogD("login");
		
		if (mContext == null) {
			LogD("mContext null");
			UserWrapper.onActionResult(facebook, UserWrapper.ACTION_RET_LOGIN_FAILED, "Activity/context hasn't been initialized");
			return;
		}
		
		 PluginWrapper.runOnMainThread(new Runnable() {
            @Override
            public void run() {
            	Session session = Session.getActiveSession();
            	if (session == null) {
	            	session = new Session.Builder(mContext).setApplicationId(appId).build();
	            	LogD("Set active session");
            	}
            	Session.setActiveSession(session);
            	isRequestPermission = false;
            	
            	if (session.isOpened()) {
            		if (scope != "" || session.getPermissions().containsAll(Arrays.asList(permissions))) {
            			LogD("Already logined!");
            			UserWrapper.onActionResult(facebook, UserWrapper.ACTION_RET_LOGIN_SUCCEED, "Already logined!");
            		}
            		else {
            			mStatusCallback.mCallByMode = SessionStatusCallback.CallByLogin;
        				mStatusCallback.mCallbackIndex = 1;
        				if (mNeedPublishPermissions) {
        					session.requestNewPublishPermissions(new Session.NewPermissionsRequest(
        							mContext, Arrays.asList(permissions)));
        				} else {
        					session.requestNewReadPermissions(new Session.NewPermissionsRequest(
        							mContext, Arrays.asList(permissions)));
        				}
            		}
            	}
            	else {
            		mStatusCallback.mCallByMode = SessionStatusCallback.CallByLogin;
            		mStatusCallback.mCallbackIndex = 1;
            		
            		if (!session.isClosed()) {
            			OpenRequest openRequest = new Session.OpenRequest(mContext);
            			openRequest.setCallback(mStatusCallback);
            			session.openForRead(openRequest);
            		}
            		else {
            			Session.openActiveSession(mContext, true, mStatusCallback);
            		}
            	}
            }
        });
	}

	public void requestPermissions() {
		LogD("Request Permissions");
		
		if (mContext == null) {
			LogD("mContext null");
			UserWrapper.onActionResult(facebook, UserWrapper.ACTION_RET_LOGIN_FAILED, "Activity/context hasn't been initialized");
			return;
		}
		
		 PluginWrapper.runOnMainThread(new Runnable() {
	        @Override
	        public void run() {
	        	Session session = Session.getActiveSession();
	        	if (session == null) {
	            	session = new Session.Builder(mContext).setApplicationId(appId).build();
	            	LogD("Set active session");
	        	}
	        	Session.setActiveSession(session);
	        	
	        	if (session.isOpened()) {
	    			mStatusCallback.mCallByMode = SessionStatusCallback.CallByLogin;
					mStatusCallback.mCallbackIndex = 1;
					if (mNeedPublishPermissions) {
						session.requestNewPublishPermissions(new Session.NewPermissionsRequest(
								mContext, Arrays.asList(permissions)));
					} else {
						session.requestNewReadPermissions(new Session.NewPermissionsRequest(
								mContext, Arrays.asList(permissions)));
					}
					isRequestPermission = true;
	        	}
	        	else
	        	{
	        		UserWrapper.onActionResult(facebook, UserWrapper.ACTION_RET_LOGIN_FAILED, "Session not open!");
	        	}
	        }
	    });
	}

	@Override
	public void logout() {
		// TODO Auto-generated method stub
		LogD("logout");
		if (isLogined())
		{
			PluginWrapper.runOnMainThread(new Runnable() {
	            @Override
	            public void run() {
	            	mStatusCallback.mCallByMode = SessionStatusCallback.CallByLogout;
	        		mStatusCallback.mCallbackIndex = 2;
					Session session = Session.getActiveSession();
					if (!session.isClosed()) {
						session.closeAndClearTokenInformation();
					}
	            }
			});
		}
		else {
			LogD("You were already logged out before calling 'logout()' method");			
		}
	}

	@Override
	public boolean isLogined() {
		// TODO Auto-generated method stub
		final Session sessionActive = Session.getActiveSession();
		if (sessionActive != null && sessionActive.isOpened()) {
			return true;
		}
		
		return false;
	}
	
	public void autoOpenActiveSession() {
		//TODO Auto-generated method stub
		LogD("Login By Token");
		isRequestPermission = true;
		
		if (mContext == null) {
			LogD("mContext null");
			UserWrapper.onActionResult(facebook, UserWrapper.ACTION_RET_LOGIN_FAILED, "Activity/context hasn't been initialized");
			return;
		}
		
		PluginWrapper.runOnMainThread(new Runnable() {
		    @Override
		    public void run() {
		    	Session session = Session.getActiveSession();
		    	if (session == null) {
		        	session = new Session.Builder(mContext).setApplicationId(appId).build();
		        	LogD("Set active session");
		    	}
		    	Session.setActiveSession(session);
		    	
		    	if (session.isOpened()) {
					UserWrapper.onActionResult(facebook, UserWrapper.ACTION_RET_LOGIN_SUCCEED, "Already logined!");
		    	}
		    	else {
		    		mStatusCallback.mCallByMode = SessionStatusCallback.CallByLogin;
		    		mStatusCallback.mCallbackIndex = 1;
		    		Session.openActiveSession(mContext, true, mStatusCallback);
		    	}
		    }
		});
	}
	
	public boolean isNetworkOnline() {
		boolean status=false;
		try{
		    ConnectivityManager cm = (ConnectivityManager) mContext.getSystemService(Context.CONNECTIVITY_SERVICE);
		    NetworkInfo netInfo = cm.getNetworkInfo(0);
		    if (netInfo != null && netInfo.getState()==NetworkInfo.State.CONNECTED) {
		        status= true;
		    }else {
		        netInfo = cm.getNetworkInfo(1);
		        if(netInfo!=null && netInfo.getState()==NetworkInfo.State.CONNECTED)
		            status= true;
		    }
		}catch(Exception e){
		    e.printStackTrace();  
		    return false;
		}
		return status;
	}
	
	public String getDeviceId() {
		try{
			return Secure.getString(mContext.getContentResolver(), Secure.ANDROID_ID);
		}catch(Exception e) {
		    e.printStackTrace();
		}
		
		return "";
	}
	
	public String getDeviceName() {
		try{
			String sDeviceName = Build.MANUFACTURER + "_" + Build.MODEL;
			return sDeviceName;
		}catch(Exception e) {
		    e.printStackTrace();
		}
		
		return "";
	}
	
	public String getOSVersion() {
		try{
			String sOsVersion = "Android " + Build.VERSION.RELEASE; 
			return sOsVersion;
		}catch(Exception e) {
		    e.printStackTrace();
		}
		
		return "";
	}
	
	public void shareFacebookLink(JSONObject shareInfo) {
		//TODO Auto-generated method stub
		LogD("Share");
		if (isLogined()) {
			final JSONObject curShareInfo = shareInfo;
			
			PluginWrapper.runOnMainThread(new Runnable() {
	            @Override
	            public void run() {
					try {
						String name = curShareInfo.getString("Param1");
						String caption = curShareInfo.getString("Param2");
						String description = curShareInfo.getString("Param3");
						String link = curShareInfo.getString("Param4");
						String picture = curShareInfo.getString("Param5");
						
						Bundle params = new Bundle();
						params.putString(Parameters.NAME, name);
					    params.putString(Parameters.CAPTION, caption);
					    params.putString(Parameters.DESCRIPTION, description);
					    params.putString(Parameters.LINK, link);
					    params.putString(Parameters.PICTURE, picture);
		        		
		        		Session session = Session.getActiveSession();
		        		Request request = new Request(session, "me/feed", params, HttpMethod.POST, new Request.Callback() {
							@Override
							public void onCompleted(Response response) {
								// TODO Auto-generated method stub
								GraphObject graphObject = response.getGraphObject();
								if (graphObject != null)
								{
									FacebookRequestError error = response.getError();
									if (error != null) {
										LogD("Share failed" + error.getException());
										UserWrapper.onActionResult(facebook, UserWrapper.ACTION_RET_LOGIN_FAILED, "Share failed!");
									}
									else {
										LogD("Share success");
										UserWrapper.onActionResult(facebook, UserWrapper.ACTION_RET_LOGIN_SUCCEED, "Share success!");
									}
								}
								else {	
									LogD("Share none");
									UserWrapper.onActionResult(facebook, UserWrapper.ACTION_RET_LOGIN_FAILED, "Share None!");
								}
							}
						});
		        		
		        		RequestAsyncTask task = new RequestAsyncTask(request);
		        		task.execute();
					} catch (JSONException e) {
						UserWrapper.onActionResult(facebook, UserWrapper.ACTION_RET_LOGIN_FAILED, "Share link parse Params error");
					}
	            }
			});
		}
	}
	
	public void shareFacebookDialog(JSONObject shareInfo) {
		//TODO Auto-generated method stub
		LogD("ShareDialog");
		if (isLogined()) {
			final JSONObject curShareInfo = shareInfo;
			
			PluginWrapper.runOnMainThread(new Runnable() {
	            @Override
	            public void run() {
	            	try {
	            		String name = curShareInfo.getString("Param1");
						String caption = curShareInfo.getString("Param2");
						String description = curShareInfo.getString("Param3");
						String link = curShareInfo.getString("Param4");
						String picture = curShareInfo.getString("Param5");
		        		
		        		Bundle params = new Bundle();
						params.putString(UserFacebook.Parameters.NAME, name);
					    params.putString(UserFacebook.Parameters.CAPTION, caption);
					    params.putString(UserFacebook.Parameters.DESCRIPTION, description);
					    params.putString(UserFacebook.Parameters.LINK, link);
					    params.putString(UserFacebook.Parameters.PICTURE, picture);
					    
					    WebDialog feedDialog = (new WebDialog.FeedDialogBuilder(mContext, Session.getActiveSession(), params))
				    		.setOnCompleteListener(new OnCompleteListener() {
								
								@Override
								 public void onComplete(Bundle values, FacebookException error){
									// TODO Auto-generated method stub
									if (error == null) {
										// When the story is posted, echo the success and the post Id.
										final String postId = values.getString("post_id");
					                    if (postId != null) {
					                    	UserFacebook.LogD("Share success!");
					                    	UserWrapper.onActionResult(facebook, UserWrapper.ACTION_RET_LOGIN_SUCCEED, "Share success!");
					                    }
					                    else {
					                        // User clicked the Cancel button
					                    	UserFacebook.LogD("Share cancelled!");
					                    	UserWrapper.onActionResult(facebook, UserWrapper.ACTION_RET_LOGIN_FAILED, "Share cancelled!");
					                    }
									}
									else if (error instanceof FacebookOperationCanceledException) {
										UserFacebook.LogD("Share cancelled!" + error.getMessage());
										UserWrapper.onActionResult(facebook, UserWrapper.ACTION_RET_LOGIN_FAILED, "Share cancelled!");
									}
									else {
										UserFacebook.LogD("Share failed" + error.getMessage());
										UserWrapper.onActionResult(facebook, UserWrapper.ACTION_RET_LOGIN_FAILED, "Share failed!");
									}
								}
							}).build();
					        
					    feedDialog.getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_ADJUST_NOTHING);
					    feedDialog.show();
	            	}
	            	catch (JSONException e) {
	            		UserWrapper.onActionResult(facebook, UserWrapper.ACTION_RET_LOGIN_FAILED, "Share dialog parse Params error");
	            	}
				}
	    	});
		}
	}
	
	public void requestDialog(JSONObject shareInfo) {
		//TODO Auto-generated method stub
		LogD("RequestDialog");
		if (isLogined()) {
			final JSONObject curShareInfo = shareInfo;
			
			PluginWrapper.runOnMainThread(new Runnable() {
	            @Override
	            public void run() {
	            	try {
	            		String message = curShareInfo.getString("Param1");
						String title = curShareInfo.getString("Param2");
						String filters = curShareInfo.getString("Param3");
						String to = curShareInfo.getString("Param3");
						String suggestions = curShareInfo.getString("Param3");
						String data = curShareInfo.getString("Param3");
								        		
		        		Bundle params = new Bundle();
						params.putString(UserFacebook.Parameters.MESSAGE, message);
					    params.putString(UserFacebook.Parameters.TITLE, title);
					    params.putString(UserFacebook.Parameters.FILTERS, filters);
					    params.putString(UserFacebook.Parameters.TO, to);
					    params.putString(UserFacebook.Parameters.SUGGESTIONS, suggestions);
					    params.putString(UserFacebook.Parameters.DATA, data);
					    
					    WebDialog requestsDialog = (new WebDialog.RequestsDialogBuilder(mContext, Session.getActiveSession(), params))
				    		.setOnCompleteListener(new OnCompleteListener() {
								
								@Override
								 public void onComplete(Bundle values, FacebookException error){
									// TODO Auto-generated method stub
									if (error != null) {
										if (error instanceof FacebookOperationCanceledException) {
											UserFacebook.LogD("Request cancelled!");
					                    	//UserWrapper.onActionResult(facebook, UserWrapper.ACTION_RET_LOGIN_FAILED, "Request cancelled!");
					                    } else {
					                    	UserFacebook.LogD("Request Network Error!");
					                    	//UserWrapper.onActionResult(facebook, UserWrapper.ACTION_RET_LOGIN_FAILED, "Request Network Error!");
					                    }
									}
									else {
										final String requestId = values.getString("request");
				                        if (requestId != null) {
				                        	UserFacebook.LogD("Request sent!");
					                    	//UserWrapper.onActionResult(facebook, UserWrapper.ACTION_RET_LOGIN_SUCCEED, "Request sent!");
				                        } else {
				                        	UserFacebook.LogD("Request cancelled!");
					                    	//UserWrapper.onActionResult(facebook, UserWrapper.ACTION_RET_LOGIN_FAILED, "Request cancelled!");
				                        }
									}
								}
							}).build();
					   
					    //requestsDialog.getWindow().setSoftInputMode(WindowManager.LayoutParams.FLAG_FULLSCREEN);
					    requestsDialog.show();
	            	}
	            	catch (JSONException e) {
	            		UserWrapper.onActionResult(facebook, UserWrapper.ACTION_RET_LOGIN_FAILED, "Request parse Params error");
	            	}
				}
	    	});
		}
	}
	
	public void openURL(JSONObject shareInfo) {
		//TODO Auto-generated method stub
		final JSONObject curShareInfo = shareInfo;
		PluginWrapper.runOnMainThread(new Runnable() {
            @Override
            public void run() {
				try {
		    		String url = curShareInfo.getString("Param1");
					
					Intent intent = new Intent(Intent.ACTION_VIEW);  
					intent.setData(Uri.parse(url));
			        mContext.startActivity(intent);
		    	}
		    	catch (JSONException e) {
		    		
		    	}
            }
		});
	}
	
	public int getAppVersionCode() {
		int version = -1;
	    try {
	        PackageInfo pInfo = mContext.getPackageManager().getPackageInfo(mContext.getPackageName(), PackageManager.GET_META_DATA);
	        version = pInfo.versionCode;
	    }catch(Exception e) {
		    e.printStackTrace();
		}
	    
	    return version;
	}
	
	@Override
	public String getSessionID() {
		// TODO Auto-generated method stub
	    String strAccessToken = "";
		if (isLogined()) {
			final Session sessionActive = Session.getActiveSession();
			strAccessToken = sessionActive.getAccessToken();
		}
		return strAccessToken;
	}
	
	@Override
	public void setDebugMode(boolean debug) {
		// TODO Auto-generated method stub
		bDebug = debug;
	}

	@Override
	public String getSDKVersion() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public String getPluginVersion() {
		// TODO Auto-generated method stub
		return null;
	}
	
	class SessionStatusCallback implements Session.StatusCallback {

		static final int CallByNull = 0;
		static final int CallByLogin = 1;
		static final int CallByLogout = 2;
				
		int mCallbackIndex = -1;
		String params = null;
		int mCallByMode = CallByNull;
		
		@Override
		public void call(Session session, SessionState state,
				Exception exception) {
			switch (mCallByMode) {
			case CallByLogin:
				if (session.isOpened()) {
					if (facebook.isRequestPermission == true) {
	        			LogD("User login succeed");
	        			UserWrapper.onActionResult(facebook, UserWrapper.ACTION_RET_LOGIN_SUCCEED, "User login succeed");
	        		}
					else
					{
						facebook.requestPermissions();
					}
        		}
        		else if(session.getState() != SessionState.OPENING) {
        			LogD("User login failed");
        			session.close();
        			UserWrapper.onActionResult(facebook, UserWrapper.ACTION_RET_LOGIN_FAILED, "User login failed");
        		}
				break;
			case CallByLogout:
				if (session.isClosed()) {
					UserWrapper.onActionResult(facebook, UserWrapper.ACTION_RET_LOGOUT_SUCCEED, "User logout succeed");
				}
				break;
			default:
				break;
			}
		}
	}

}