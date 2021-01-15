package com.huahuico.mynatiaveapplication.compose

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.activity.result.ActivityResultCallback
import androidx.activity.result.ActivityResultLauncher
import androidx.activity.result.ActivityResultRegistry
import androidx.activity.result.contract.ActivityResultContract
import androidx.compose.foundation.Image
import androidx.compose.foundation.Text
import androidx.compose.foundation.isSystemInDarkTheme
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.Button
import androidx.compose.material.Divider
import androidx.compose.material.MaterialTheme
import androidx.compose.material.Surface
import androidx.compose.runtime.Composable
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.res.imageResource
import androidx.compose.ui.text.style.TextOverflow
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.fragment.app.Fragment
import androidx.lifecycle.ViewModelProvider
import com.huahuico.mynatiaveapplication.R
import com.huahuico.mynatiaveapplication.databinding.ComposeFragmentBinding

class ComposeFragment : Fragment() {

    companion object {
        fun newInstance() = ComposeFragment()
    }
    private var _bindingView : ComposeFragmentBinding? = null
    private lateinit var viewModel: ComposeViewModel

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        _bindingView = ComposeFragmentBinding.inflate(inflater, container, false)
        return _bindingView?.root
    }

    override fun onActivityCreated(savedInstanceState: Bundle?) {
        super.onActivityCreated(savedInstanceState)
        viewModel = ViewModelProvider(this).get(ComposeViewModel::class.java)
        // TODO: Use the ViewModel
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        _bindingView?.let {
            it.composeView.setContent {
//                Greeting(name = "Android")
                MyContainer {
                    NewsStory()
                    Divider(color = Color.Transparent, thickness = 32.dp)
                    Counter()
                }
            }
        }
    }
    @Composable
    fun MyContainer(content : @Composable () -> Unit) {
        content()
    }
    @Composable
    fun Greeting(name : String) {
        Surface(color = Color.Yellow) {
            Text(text = "Hello $name", modifier = Modifier.padding(24.dp))
        }
    }

    @Preview(showBackground = true, name = "Text Preview")
    @Composable
    fun DefaultPreview() {
        Greeting(name = "Android")
//        NewsStory()
        codeLab()
        Divider(color = Color.Transparent, thickness = 32.dp)
        Counter()
    }
    @Composable
    fun NewsStory() {
        val imageModifier = Modifier
                .preferredHeight(180.dp)
                .fillMaxWidth()
                .clip(shape = RoundedCornerShape(4.dp))
        val typography = MaterialTheme.typography
        Column(modifier = Modifier.padding(16.dp)) {
            Image(
                bitmap = imageResource(id = R.drawable.header),
                modifier = imageModifier,
                contentScale = ContentScale.Crop)
            Spacer(modifier = Modifier.preferredHeight(16.dp))
            Text("A day in Shark Fin Cove", style = typography.h6, maxLines = 2, overflow = TextOverflow.Ellipsis)
            Text("Davenport, California", style = typography.body2)
            Text("December 2018", style = typography.body2)
            Greeting("Android")
            Divider(color = Color.Black)
            Greeting(name = "there")
        }
    }
    @Composable
    fun BasicsCodelabTheme(darkTheme: Boolean = isSystemInDarkTheme(), content : @Composable () -> Unit) {
        //TODO
    }
    @Composable
    fun codeLab() {
        BasicsCodelabTheme {
            Surface(color = MaterialTheme.colors.background) {
                Greeting(name = "Android")
            }
        }
    }
    @Composable
    fun Counter() {
        val count = remember { mutableStateOf(0) }
        Button(onClick = {count.value++}, modifier = Modifier
                .height(50.dp)
                .width(180.dp)) {
            Text(text = "I've been clicked ${count.value} times")
        }
    }
}