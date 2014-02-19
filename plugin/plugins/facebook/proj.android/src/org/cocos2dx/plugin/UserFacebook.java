package org.cocos2dx.plugin;

import java.util.Arrays;
import java.util.Hashtable;
import java.util.List;

import org.json.JSONException;
import org.json.JSONObject;

import com.facebook.AccessToken;
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
import android.os.Bundle;
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
		    		appId = curCPInfo.get("FacebookAppId");
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
            		String strAccessToken = session.getAccessToken();
            		
            		if(strAccessToken == "") {
	            		if (!session.isClosed()) {
	            			OpenRequest openRequest = new Session.OpenRequest(mContext);
	            			if (permissions != null) {
	            				openRequest.setPermissions(Arrays.asList(permissions));
	            			}
	            			openRequest.setCallback(mStatusCallback);
	            			
	            			if (mNeedPublishPermissions) {
	            				session.openForPublish(openRequest);
	            			}
	            			else {
	            				session.openForRead(openRequest);
	            			}
	            		}
	            		else {
	            			Session.openActiveSession(mContext, true, mStatusCallback);
	            		}
            		}
            		else {
            			LogD("Access token:" + session.getAccessToken());
            			if (mNeedPublishPermissions) {
            				AccessToken access = AccessToken.createFromExistingAccessToken(strAccessToken, null, null, null, Arrays.asList(permissions));
            				session.open(access, mStatusCallback);
            			}
            			else {
            				AccessToken access = AccessToken.createFromExistingAccessToken(strAccessToken, null, null, null, null);
            				session.open(access, mStatusCallback);
            			}
            		}
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
	
	public void shareFacebookLink(JSONObject shareInfo) {
		//TODO Auto-generated method stub
		LogD("Share");
		if (isLogined()) {
			final JSONObject curShareInfo = shareInfo;
			LogD("shareFacebookApi invoked! event : " + curShareInfo.toString());
			
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
						UserWrapper.onActionResult(facebook, UserWrapper.ACTION_RET_LOGIN_FAILED, "Parse Params error");
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
			LogD("shareFacebookDialog invoked! event : " + curShareInfo.toString());
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
	            		UserWrapper.onActionResult(facebook, UserWrapper.ACTION_RET_LOGIN_FAILED, "Parse Params error");
	            	}
				}
	    	});
		}
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
		int mCallbackIndex = -1;

		String params = null;

		int mCallByMode = CallByNull;
		static final int CallByNull = 0;
		static final int CallByLogin = 1;
		static final int CallByLogout = 2;
		
		@Override
		public void call(Session session, SessionState state,
				Exception exception) {
			switch (mCallByMode) {
			case CallByLogin:
				if (session.isOpened()) {
            		LogD("User login succeed");
        			UserWrapper.onActionResult(facebook, UserWrapper.ACTION_RET_LOGIN_SUCCEED, "User login succeed");
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