#if UNITY_EDITOR
using System;
using System.Collections;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using UnityEngine;
using UnityEngine.Rendering;
using UnityEditor;


public class TweetMediaPackaging
{
    [MenuItem("Assets/TweetMedia/Make Package")]
    public static void ExportPackage_Gif()
    {
        string[] files = new string[]
        {
"Assets/TweetMedia/Examples/Example.unity",
"Assets/TweetMedia/Prefabs/TweetMedia.prefab",
"Assets/TweetMedia/Scripts/TweetMedia.cs",
"Assets/TweetMedia/Scripts/TweetMediaGUI.cs",
"Assets/TweetMedia/Scripts/TweetMediaPlugin.cs",
"Assets/Plugins/x86_64/TweetMedia.dll",
"Assets/Plugins/x86/TweetMedia.dll",
        };
        AssetDatabase.ExportPackage(files, "TweetMedia.unitypackage");
    }

}
#endif // UNITY_EDITOR
